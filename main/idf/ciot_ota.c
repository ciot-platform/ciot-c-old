/**
 * @file ciot_ota.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_OTA

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
#include "esp_encrypted_img.h"
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB

#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#include "ciot_ota.h"
#include "ciot_tasks.h"

#define CIOT_OTA_EVENT_BIT_DONE BIT0

static const char *TAG = "hdr_ota";

static void ciot_ota_task(void *pvParameters);
static void ciot_ota_task_fatal_error(ciot_ota_t *ota);
static void ciot_ota_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
static ciot_err_t ciot_ota_validate_image_header(esp_app_desc_t *new_app_info);
static ciot_err_t ciot_ota_decrypt_cb(decrypt_cb_arg_t *args, void *user_ctx);
extern const char rsa_private_pem_start[] asm("_binary_private_pem_start");
extern const char rsa_private_pem_end[] asm("_binary_private_pem_end");
#endif // CONFIG_ESP_HTTPS_OTA_DECRYPT_CB

ciot_err_t ciot_ota_start(ciot_ota_t *ota)
{
#ifndef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    if (ota->config.encrypted)
    {
        return CIOT_ERR_FEATURE_NOT_SUPPORTED;
    }
#endif

    if (ota->status.state != CIOT_OTA_STATE_IDLE)
    {
        return CIOT_ERR_INVALID_STATE;
    }

    if (ota->event_group == NULL)
    {
        ota->event_group = xEventGroupCreate();
    }

    ota->buffer = malloc(CIOT_OTA_BUFFSIZE);
    ota->status.state = CIOT_OTA_STATE_INIT;
    ota->status.error = CIOT_ERR_OK;

    ESP_ERROR_CHECK(esp_event_handler_register(ESP_HTTPS_OTA_EVENT, ESP_EVENT_ANY_ID, ciot_ota_event_handler, ota));

    xTaskCreatePinnedToCore(
        ciot_ota_task,
        "ciot_ota_task",
        CIOT_OTA_TASK_STACK_SIZE,
        ota,
        CIOT_OTA_TASK_TASK_PRIORITY,
        ota->task,
        CIOT_OTA_TASK_CORE_ID);
    xEventGroupWaitBits(ota->event_group, CIOT_OTA_EVENT_BIT_DONE, pdTRUE, pdFALSE, ota->config.timeout / portTICK_PERIOD_MS);
    return ota->status.error;
}

static void ciot_ota_task(void *pvParameters)
{
    ciot_ota_t *ota = (ciot_ota_t *)pvParameters;

    ESP_LOGI(TAG, "OTA application start.");
    ESP_LOGI(TAG, "config: url:%s", ota->config.url);

    esp_http_client_config_t http_client_config = {
        .url = (const char *)ota->config.url,
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        .crt_bundle_attach = esp_crt_bundle_attach
#endif
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &http_client_config,
    };

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
    esp_decrypt_handle_t decrypt_handle = NULL;
    if (ota->config.encrypted)
    {
        esp_decrypt_cfg_t decrypt_cfg = {};
        decrypt_cfg.rsa_pub_key = rsa_private_pem_start;
        decrypt_cfg.rsa_pub_key_len = rsa_private_pem_end - rsa_private_pem_start;

        decrypt_handle = esp_encrypted_img_decrypt_start(&decrypt_cfg);
        if (!decrypt_handle)
        {
            ESP_LOGI(TAG, "Failed to start decrypt");
            ota->status.error = ESP_ERR_OTA_VALIDATE_FAILED;
            ciot_ota_task_fatal_error(ota);
        }

        ota_config.decrypt_cb = ciot_ota_decrypt_cb;
        ota_config.decrypt_user_ctx = (void *)decrypt_handle;
    }
#endif

    if (ota->callbacks.on_ota_started != NULL)
    {
        ota->callbacks.on_ota_started();
    }

    ota->handle = NULL;
    ota->status.error = esp_https_ota_begin(&ota_config, &ota->handle);
    if (ota->status.error != ESP_OK)
    {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed!");
        ciot_ota_task_fatal_error(ota);
    }

    while (1)
    {
        ota->status.error = esp_https_ota_perform(ota->handle);
        if (ota->status.error != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        {
            break;
        }
    }

    if (!esp_https_ota_is_complete_data_received(ota->handle))
    {
        ESP_LOGE(TAG, "Complete data was not received.");
    }
    else
    {
#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
        if (ota->config.encrypted && decrypt_handle != NULL)
        {
            ota->status.error = esp_encrypted_img_decrypt_end(decrypt_handle);
            if (ota->status.error != ESP_OK)
            {
                esp_https_ota_abort(ota->handle);
                ESP_LOGE(TAG, "upgrade failed");
                ciot_ota_task_fatal_error(ota);
                return;
            }
        }
#endif
        ota->status.error = esp_https_ota_finish(ota->handle);
        if (ota->status.error == ESP_OK && ota->config.restart)
        {
            ESP_LOGI(TAG, "Prepare to restart system!");

            if (ota->callbacks.on_ota_completed != NULL)
            {
                ota->callbacks.on_ota_completed();
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_restart();
        }
        if (ota->status.error != ESP_OK)
        {
            ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", ota->status.error);
            ciot_ota_task_fatal_error(ota);
        }
    }

    xEventGroupSetBits(ota->event_group, CIOT_OTA_EVENT_BIT_DONE);

    (void)vTaskDelete(NULL);
}

static void __attribute__((noreturn)) ciot_ota_task_fatal_error(ciot_ota_t *ota)
{
    ESP_LOGE(TAG, "Exiting task due to fatal error...");

    ota->status.state = CIOT_OTA_STATE_ERROR;
    if (ota->callbacks.on_ota_error != NULL)
    {
        ota->callbacks.on_ota_error(ota->status.error);
    }
    xEventGroupSetBits(ota->event_group, CIOT_OTA_EVENT_BIT_DONE);

    (void)vTaskDelete(NULL);

    while (1)
    {
        ;
    }
}

static void ciot_ota_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if(event_base == ESP_HTTPS_OTA_EVENT)
    {
        ciot_ota_t *ota = (ciot_ota_t*)arg;
        switch (event_id)
        {
            case ESP_HTTPS_OTA_START:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_START");
                ota->status.state = CIOT_OTA_STATE_START;
                break;
            case ESP_HTTPS_OTA_CONNECTED:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_CONNECTED");
                ota->status.state = CIOT_OTA_STATE_CONNECTED;
                break;
            case ESP_HTTPS_OTA_GET_IMG_DESC:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_GET_IMG_DESC");
                ota->status.state = CIOT_OTA_STATE_CHECKING_DATA;
                break;
            case ESP_HTTPS_OTA_VERIFY_CHIP_ID:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_VERIFY_CHIP_ID");
                ota->status.state = CIOT_OTA_STATE_CHECKING_DATA;
                break;
            case ESP_HTTPS_OTA_DECRYPT_CB:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_DECRYPT_CB");
                ota->status.state = CIOT_OTA_STATE_DECRYPTING;
                break;
            case ESP_HTTPS_OTA_WRITE_FLASH:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_WRITE_FLASH");
                ota->status.state = CIOT_OTA_STATE_FLASHING;
                ota->status.image_size = esp_https_ota_get_image_size(ota->handle);
                ota->status.image_read = esp_https_ota_get_image_len_read(ota->handle);
                break;
            case ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION");
                ota->status.state = CIOT_OTA_STATE_UPDATE_BOOT_PARTITION;
                break;
            case ESP_HTTPS_OTA_FINISH:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_FINISH");
                ota->status.state = CIOT_OTA_STATE_DONE;
                xEventGroupSetBits(ota->event_group, CIOT_OTA_EVENT_BIT_DONE);
                break;
            case ESP_HTTPS_OTA_ABORT:
                ESP_LOGI(TAG, "ESP_HTTPS_OTA_ABORT");
                ota->status.state = CIOT_OTA_STATE_ERROR;
                xEventGroupSetBits(ota->event_group, CIOT_OTA_EVENT_BIT_DONE);
                break;
        default:
            break;
        }
    }
}

#ifdef CONFIG_ESP_HTTPS_OTA_DECRYPT_CB
static ciot_err_t ciot_ota_validate_image_header(esp_app_desc_t *new_app_info)
{
    esp_app_desc_t running_app_info;
    esp_app_desc_t invalid_app_info;

    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    const esp_partition_t *last_invalid_partition = esp_ota_get_last_invalid_partition();

    esp_ota_get_partition_description(running_partition, &running_app_info);
    esp_ota_get_partition_description(last_invalid_partition, &invalid_app_info);

    bool version_is_invalid = memcmp(invalid_app_info.version, new_app_info->version, sizeof(new_app_info->version)) == 0;
    if (last_invalid_partition != NULL && version_is_invalid)
    {
        ESP_LOGE(TAG, "The received version is marked as invalid");
        return CIOT_ERR_INVALID_VERSION;
    }

    bool version_is_same = memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0;
    if (version_is_same)
    {
        ESP_LOGE(TAG, "The received version is equal current version");
        return CIOT_ERR_SAME_VERSION;
    }

    return CIOT_ERR_OK;
}

static ciot_err_t ciot_ota_decrypt_cb(decrypt_cb_arg_t *args, void *user_ctx)
{
    if (args == NULL || user_ctx == NULL)
    {
        ESP_LOGE(TAG, "hg_ota_app_decrypt_cb: Invalid argument");
        return ESP_ERR_INVALID_ARG;
    }

    ciot_err_t err;
    pre_enc_decrypt_arg_t pargs = {};

    pargs.data_in = args->data_in;
    pargs.data_in_len = args->data_in_len;

    err = esp_encrypted_img_decrypt_data((esp_decrypt_handle_t *)user_ctx, &pargs);
    if (err != CIOT_ERR_OK && err != ESP_ERR_NOT_FINISHED)
    {
        return err;
    }

    static bool image_header_was_checked = false;
    if (pargs.data_out_len > 0)
    {
        args->data_out = pargs.data_out;
        args->data_out_len = pargs.data_out_len;
        if (image_header_was_checked == false)
        {
            const int header_size = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
            const int app_descr_size = sizeof(esp_app_desc_t);
            assert(args->data_out_len >= header_size + app_descr_size);

            esp_app_desc_t *app_info = (esp_app_desc_t *)&args->data_out[header_size];
            err = ciot_ota_validate_image_header(app_info);
            if (err != CIOT_ERR_OK)
            {
                free(pargs.data_out);
            }
            return err;
        }
    }
    else
    {
        args->data_out_len = 0;
    }

    return CIOT_ERR_OK;
}
#endif

#endif