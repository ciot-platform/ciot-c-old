/**
 * @file ciot_settings.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_config.h"
#include "ciot_settings.h"
#include "ciot_storage.h"

ciot_err_t ciot_settings_save(void)
{
    ciot_err_t err = CIOT_ERR_OK;
    ciot_settings_t settings;

#if CIOT_CONFIG_FEATURE_WIFI
    err = ciot_wifi_get_config(CIOT_WIFI_IF_AP, &settings.wifi_ap);
    if (err != CIOT_ERR_OK)
    {
        CIOT_ERROR_PRINT(ciot_storage_save_data(&settings.wifi_ap, sizeof(settings.wifi_ap), CIOT_CONFIG_WIFI_AP_FILENAME));
    }
    err = ciot_wifi_get_config(CIOT_WIFI_IF_STA, &settings.wifi_sta);
    if (err != CIOT_ERR_OK)
    {
        CIOT_ERROR_PRINT(ciot_storage_save_data(&settings.wifi_sta, sizeof(settings.wifi_sta), CIOT_CONFIG_WIFI_STA_FILENAME));
    }
#endif

#if CIOT_CONFIG_FEATURE_NTP
    err = ciot_ntp_get_config(&settings.ntp);
    if (err != CIOT_ERR_OK)
    {
        CIOT_ERROR_PRINT(ciot_storage_save_data(&settings.ntp, sizeof(settings.ntp), CIOT_CONFIG_NTP_FILENAME));
    }
#endif

#if CIOT_CONFIG_FEATURE_MQTT
    err = ciot_mqtt_get_config(&settings.mqtt);
    if (err != CIOT_ERR_OK)
    {
        CIOT_ERROR_PRINT(ciot_storage_save_data(&settings.ntp, sizeof(settings.ntp), CIOT_CONFIG_NTP_FILENAME));
    }
#endif

    return err;
}

ciot_err_t ciot_settings_load(ciot_settings_t *settings)
{
    ciot_err_t err = CIOT_ERR_OK;

#if CIOT_CONFIG_FEATURE_WIFI
    err = ciot_storage_load_data(&settings->wifi_ap, sizeof(settings->wifi_ap), CIOT_CONFIG_WIFI_AP_FILENAME);
    CIOT_ERROR_PRINT(err);
    err = ciot_storage_load_data(&settings->wifi_sta, sizeof(settings->wifi_sta), CIOT_CONFIG_WIFI_STA_FILENAME);
    CIOT_ERROR_PRINT(err);
#endif

#if CIOT_CONFIG_FEATURE_NTP
    err = ciot_storage_load_data(&settings->ntp, sizeof(settings->ntp), CIOT_CONFIG_NTP_FILENAME);
    CIOT_ERROR_PRINT(err);
#endif

#if CIOT_CONFIG_FEATURE_MQTT
    err = ciot_storage_load_data(&settings->mqtt, sizeof(settings->mqtt), CIOT_CONFIG_MQTT_FILENAME);
    CIOT_ERROR_PRINT(err);
#endif

    return err;
}

ciot_err_t ciot_settings_clear(void)
{
    ciot_err_t err = CIOT_ERR_OK;
    ciot_err_t ret = err;

#if CIOT_CONFIG_FEATURE_WIFI
    err = ciot_storage_remove_data(CIOT_CONFIG_WIFI_AP_FILENAME);
    if(err != CIOT_ERR_OK)
    {
        ret = err;
        CIOT_ERROR_PRINT(ret);
    }
    err = ciot_storage_remove_data(CIOT_CONFIG_WIFI_STA_FILENAME);
    if(err != CIOT_ERR_OK)
    {
        ret = err;
        CIOT_ERROR_PRINT(ret);
    }
#endif

#if CIOT_CONFIG_FEATURE_NTP
    err = ciot_storage_remove_data(CIOT_CONFIG_NTP_FILENAME);
    if(err != CIOT_ERR_OK)
    {
        ret = err;
        CIOT_ERROR_PRINT(ret);
    }
#endif

#if CIOT_CONFIG_FEATURE_MQTT
    err = ciot_storage_remove_data(CIOT_CONFIG_MQTT_FILENAME);
    if(err != CIOT_ERR_OK)
    {
        ret = err;
        CIOT_ERROR_PRINT(ret);
    }
#endif

    return ret;
}
