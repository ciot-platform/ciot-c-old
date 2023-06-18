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

typedef struct ciot_settings
{
    ciot_wifi_config_t wifi;
    ciot_ntp_config_t ntp;
} ciot_settings_t;

ciot_err_t ciot_settings_save(void);
ciot_err_t ciot_settings_load(ciot_settings_t *settings);
ciot_err_t ciot_settings_clear(void);
