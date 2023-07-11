/**
 * @file ciot_settings.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_ntp.h"
#include "ciot_wifi.h"
#include "ciot_mqtt.h"

typedef struct ciot_settings
{
    ciot_wifi_config_t wifi_ap;
    ciot_wifi_config_t wifi_sta;
    ciot_ntp_config_t ntp;
    ciot_mqtt_config_t mqtt;
} ciot_settings_t;

ciot_err_t ciot_settings_save(ciot_settings_t *settings);
ciot_err_t ciot_settings_load(ciot_settings_t *settings);
ciot_err_t ciot_settings_clear(void);
