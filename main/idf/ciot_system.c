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

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_SYS

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

static const char *TAG = "ciot_system";

static void reset_task(void *pvParameters);

ciot_err_t ciot_system_get_status(ciot_system_status_t *status)
{
    ciot_system_status_t system = {
        .time = time(NULL),
        .memory = esp_get_free_heap_size(),
        .sys_err = ciot_system_get_err_code(),
        .sys_status = ciot_system_get_status_code(),
        .lifetime = esp_timer_get_time() / 1000000,
    };
    *status = system;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_get_info(ciot_system_info_t *info)
{
    ciot_system_info_t system = {
        .device = CIOT_CONFIG_DEVICE_TYPE,
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

ciot_err_t ciot_system_reset(void)
{
    xTaskCreate(&reset_task, "reset_task", 2048, NULL, 5, NULL);
    return CIOT_ERR_OK;
}

static void reset_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}

#endif