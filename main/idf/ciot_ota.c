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

static const char *TAG = "hdr_ota";

static void ciot_ota_task(void *pvParameters);
static void ciot_ota_task_fatal_error(ciot_ota_t *ota);

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

    ota->buffer = malloc(CIOT_OTA_BUFFSIZE);
    ota->status.state = CIOT_OTA_STATE_IN_PROGRESS;
    ota->status.error = CIOT_ERR_OK;

    xTaskCreatePinnedToCore(
        ciot_ota_task,
        "ciot_ota_task",
        CIOT_OTA_TASK_STACK_SIZE,
        ota,
        CIOT_OTA_TASK_TASK_PRIORITY,
        ota->task,
        CIOT_OTA_TASK_CORE_ID);

    return CIOT_ERR_OK;
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

    esp_https_ota_handle_t https_ota_handle = NULL;
    ota->status.error = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (ota->status.error != ESP_OK)
    {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed!");
        ciot_ota_task_fatal_error(ota);
    }

    while (1)
    {
        ota->status.error = esp_https_ota_perform(https_ota_handle);
        if (ota->status.error != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        {
            break;
        }
    }

    if (!esp_https_ota_is_complete_data_received(https_ota_handle))
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
                esp_https_ota_abort(https_ota_handle);
                ESP_LOGE(TAG, "upgrade failed");
                ciot_ota_task_fatal_error(ota);
                return;
            }
        }
#endif

        ota->status.error = esp_https_ota_finish(https_ota_handle);
        if (ota->status.error == ESP_OK)
        {
            ESP_LOGI(TAG, "Prepare to restart system!");

            if (ota->callbacks.on_ota_completed != NULL)
            {
                ota->callbacks.on_ota_completed();
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_restart();
        }
        else
        {
            ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", ota->status.error);
            ciot_ota_task_fatal_error(ota);
        }
    }

    esp_https_ota_abort(https_ota_handle);
    ESP_LOGE(TAG, "upgrade failed");
    ciot_ota_task_fatal_error(ota);
}

static void __attribute__((noreturn)) ciot_ota_task_fatal_error(ciot_ota_t *ota)
{
    ESP_LOGE(TAG, "Exiting task due to fatal error...");

    if (ota->callbacks.on_ota_error != NULL)
    {
        ota->callbacks.on_ota_error(ota->status.error);
    }

    (void)vTaskDelete(NULL);

    while (1)
    {
        ;
    }
}

// static void ciot_ota_task(void *pvParameter)
// {
//     ciot_ota_t *ota = (ciot_ota_t*)pvParameter;
//     esp_ota_handle_t update_handle = 0;
//     const esp_partition_t *update_partition = NULL;

//     ESP_LOGI(TAG, "OTA application start.");
//     ESP_LOGI(TAG, "config: url:%s", ota->config.url);

//     update_partition = esp_ota_get_next_update_partition(NULL);
//     assert(update_partition != NULL);

//     esp_http_client_config_t http_client_config = {
//         .url = (const char*)ota->config.url,
// #ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
//         .crt_bundle_attach = esp_crt_bundle_attach
// #endif
//     };
//     esp_http_client_handle_t client = esp_http_client_init(&http_client_config);

//     if(client == NULL)
//     {
//         ESP_LOGE(TAG, "Http client init error");
//         ota->status.error = CIOT_ERR_INIT;
//         ciot_ota_task_fatal_error(ota);
//         return;
//     }

//     ota->status.error = esp_http_client_open(client, 0);
//     if(ota->status.error != CIOT_ERR_OK)
//     {
//         ciot_ota_task_fatal_error(ota);
//         esp_http_client_cleanup(client);
//         return;
//     }

//     esp_http_client_fetch_headers(client);
//     ciot_ota_http_read(ota, client, update_handle, update_partition);
//     ota->status.error = esp_ota_end(update_handle);
//     if(ota->status.error != CIOT_ERR_OK)
//     {
//         ESP_LOGE(TAG, "%s", esp_err_to_name(ota->status.error));
//         esp_http_client_close(client);
//         esp_http_client_cleanup(client);
//         ciot_ota_task_fatal_error(ota);
//     }

//     ota->status.error = esp_ota_set_boot_partition(update_partition);
//     if (ota->status.error != ESP_OK)
//     {
//         ESP_LOGE(TAG, "%s", esp_err_to_name(ota->status.error));
//         esp_http_client_close(client);
//         esp_http_client_cleanup(client);
//         ciot_ota_task_fatal_error(ota);
//     }

//     if (ota->callbacks.on_ota_completed != NULL)
//     {
//         ota->callbacks.on_ota_completed();
//     }

//     ESP_LOGI(TAG, "Prepare to restart system!");
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//     esp_restart();
//     return;
// }

// static void ciot_ota_http_read(ciot_ota_t *ota, esp_http_client_handle_t client, esp_ota_handle_t update_handle, const esp_partition_t *update_partition)
// {
//     int binary_file_lenght = 0;
//     bool image_header_was_checked = false;
//     esp_app_desc_t new_app_info;

//     while (true)
//     {
//         int data_read = esp_http_client_read(client, ota->buffer, CIOT_OTA_BUFFSIZE);
//         if(data_read < 0)
//         {
//             ESP_LOGE(TAG, "Http client read error");
//             ota->status.error = CIOT_ERR_READ;
//             esp_http_client_close(client);
//             esp_http_client_cleanup(client);
//             ciot_ota_task_fatal_error(ota);
//         }
//         else if(data_read > 0)
//         {
//             const int expected_size = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t);

//             if(image_header_was_checked == false && data_read > expected_size)
//             {
//                 const int header_size = sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t);
//                 const int app_descr_size = sizeof(esp_app_desc_t);

//                 memcpy(&new_app_info, &ota->buffer[header_size], app_descr_size);
//                 ota->status.error = ciot_ota_validate_image_header(&new_app_info);
//                 if(ota->status.error != CIOT_ERR_OK && ota->config.force == false)
//                 {
//                     esp_http_client_close(client);
//                     esp_http_client_cleanup(client);
//                     ciot_ota_task_fatal_error(ota);
//                 }

//                 image_header_was_checked = true;

//                 if(ota->callbacks.on_ota_started != NULL)
//                 {
//                     ota->callbacks.on_ota_started();
//                 }

//                 ota->status.error = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
//                 if(ota->status.error != CIOT_ERR_OK)
//                 {
//                     ESP_LOGE(TAG, "%s", esp_err_to_name(ota->status.error));
//                     esp_http_client_close(client);
//                     esp_http_client_cleanup(client);
//                     ciot_ota_task_fatal_error(ota);
//                 }
//             }

//             if(data_read < expected_size)
//             {
//                 ESP_LOGE(TAG, "Received data size is incorrect");
//                 ota->status.error = CIOT_ERR_INVALID_SIZE;
//                 esp_http_client_close(client);
//                 esp_http_client_cleanup(client);
//                 ciot_ota_task_fatal_error(ota);
//             }

//             ota->status.error = esp_ota_write(update_handle, (const void *)ota->buffer, data_read);
//             if(ota->status.error != CIOT_ERR_OK)
//             {
//                 ESP_LOGE(TAG, "%s", esp_err_to_name(ota->status.error));
//                 esp_http_client_close(client);
//                 esp_http_client_cleanup(client);
//                 ciot_ota_task_fatal_error(ota);
//             }

//             binary_file_lenght += data_read;
//         }
//         else if(data_read == 0)
//         {
//             break;
//         }
//     }
// }

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