/**
 * @file ciot_ntp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_ntp.h"
#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_NTP

#include <stdbool.h>
#include <string.h>

#include "esp_sntp.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"


#define CIOT_NTP_EVENT_BIT_DONE BIT0

typedef struct ciot_ntp
{
    ciot_ntp_config_t config;
    ciot_ntp_status_t status;
    EventGroupHandle_t event_group;
} ciot_ntp_t;

static ciot_ntp_t ntp;
static const char *TAG = "ciot_ntp";

static void ciot_ntp_sync_notification_cb(struct timeval *tv);

ciot_err_t ciot_ntp_set_config(ciot_ntp_config_t *conf)
{
    ESP_LOGI(TAG, "config: servers:<%s><%s><%s> timeout:%d", conf->server1, conf->server2, conf->server3, conf->timeout);

    memcpy(&ntp.config, conf, sizeof(ntp.config));

    if(ntp.status.init)
    {
        sntp_stop();
    }
    else
    {
        ntp.event_group = xEventGroupCreate();
    }

    if(ntp.config.sync_interval == 0)
    {
        ntp.config.sync_interval = 3600 * 1000;
    }

    if (ntp.config.server1 && ntp.config.server1[0] != '\0')
    {
        sntp_setservername(0, ntp.config.server1);
    }

    if (ntp.config.server2 && ntp.config.server2[0] != '\0')
    {
        sntp_setservername(1, ntp.config.server2);
    }

    if (ntp.config.server3 && ntp.config.server3[0] != '\0')
    {
        sntp_setservername(2, ntp.config.server3);
    }

    if (ntp.config.timezone[0] == '\0')
    {
        sprintf(ntp.config.timezone, CIOT_CONFIG_DEFAULT_TIMEZONE);
    }

    sntp_setoperatingmode(ntp.config.op_mode);
    sntp_set_sync_interval(ntp.config.sync_interval);
    sntp_set_sync_mode(ntp.config.sync_mode);
    sntp_set_time_sync_notification_cb(ciot_ntp_sync_notification_cb);

    sntp_init();
    ntp.status.init = true;

    setenv("TZ", ntp.config.timezone, 1);
    tzset();

    if(conf->timeout != 0)
    {
        xEventGroupWaitBits(ntp.event_group, CIOT_NTP_EVENT_BIT_DONE, pdTRUE, pdFALSE, conf->timeout / portTICK_PERIOD_MS);
        return ntp.status.sync ? CIOT_ERR_OK : CIOT_ERR_FAIL;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_get_config(ciot_ntp_config_t *config)
{
    *config = ntp.config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_get_status(ciot_ntp_status_t *status)
{
    ntp.status.state = sntp_get_sync_status();
    *status = ntp.status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_get_info(ciot_ntp_info_t *info)
{
    info->available = CIOT_CONFIG_FEATURE_NTP;
    return CIOT_ERR_OK;
}

static void ciot_ntp_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "NTP Sync OK");
    ntp.status.sync = true;
    ntp.status.sync_count++;
    ntp.status.last_sync = time(NULL);
    xEventGroupSetBits(ntp.event_group, CIOT_NTP_EVENT_BIT_DONE);
}

#endif
