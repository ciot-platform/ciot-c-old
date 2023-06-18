/**
 * @file ciot_system.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <sys/time.h>

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "ciot_system.h"
#include "ciot_storage.h"
#include "ciot_settings.h"
#include "ciot_config.h"

typedef struct ciot_system
{
    ciot_system_config_t config;
    uint32_t err_code;
    uint32_t status_code;
} ciot_system_t;

static ciot_system_t this;
static const char *TAG = "ciot_system";

static void reset_task(void *pvParameters);

ciot_err_t ciot_system_set_config(ciot_system_config_t *conf)
{
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_get_config(ciot_system_config_t *config)
{
    *config = this.config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_get_status(ciot_system_status_t *status)
{
    ciot_system_status_t system = {
        .time = time(NULL),
        .memory = esp_get_free_heap_size(),
        .err = this.err_code,
        .status = this.status_code,
        .lifetime = esp_timer_get_time() / 1000000,
    };
    *status = system;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_get_info(ciot_system_info_t *info)
{
    ciot_system_info_t system = {
        .type = CIOT_CONFIG_HARDWARE_TYPE,
        .os = CIOT_SYSTEM_OS_FREERTOS,
        .version = CIOT_CONFIG_VERSION,
        .date = CIOT_CONFIG_DATE,
        .board = CIOT_CONFIG_BOARD,
        .mcu = CIOT_CONFIG_MCU,
        .storage = ciot_storage_get_size(),
        .features = CIOT_CONFIG_FEATURES,
    };
    *info = system;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_set_err(uint32_t code)
{
    this.err_code = code;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_set_status(uint32_t code)
{
    this.status_code = code;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_process_request(ciot_system_request_t request)
{
    switch (request)
    {
    case CIOT_SYSTEM_REQUEST_RESTART:
        ESP_LOGI(TAG, "CIOT_SYSTEM_REQUEST_RESTART");
        xTaskCreate(&reset_task, "reset_task", 2048, NULL, 5, NULL);
        return CIOT_ERR_OK;
    case CIOT_SYSTEM_REQUEST_SAVE_SETTINGS:
        ESP_LOGI(TAG, "CIOT_SYSTEM_REQUEST_SAVE_SETTINGS");
        return ciot_settings_save();
    case CIOT_SYSTEM_REQUEST_CLEAR_SETTINGS:
        ESP_LOGI(TAG, "CIOT_SYSTEM_REQUEST_CLEAR_SETTINGS");
        return ciot_settings_clear();
    default:
        return CIOT_ERR_INVALID_REQUEST;
    }
}

static void reset_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}