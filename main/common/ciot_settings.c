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

#include "ciot_settings.h"
#include "ciot_storage.h"

ciot_err_t ciot_settings_save(void)
{
    ciot_err_t err = CIOT_ERR_OK;
    ciot_settings_t settings;

    ciot_wifi_get_config(CIOT_WIFI_TYPE_STA, &settings.wifi);
    ciot_ntp_get_config(&settings.ntp);

    err = ciot_storage_save_data(&settings.wifi, sizeof(settings.wifi), CIOT_SETTINGS_WIFI_FILENAME);
    if(err)
        return err;

    err = ciot_storage_save_data(&settings.ntp, sizeof(settings.ntp), CIOT_SETTINGS_NTP_FILENAME);
    if(err)
        return err;
    
    return err;
}

ciot_err_t ciot_settings_load(ciot_settings_t *settings)
{
    ciot_err_t err = CIOT_ERR_OK;
    
    err = ciot_storage_load_data(&settings->wifi, sizeof(settings->wifi), CIOT_SETTINGS_WIFI_FILENAME);
    if(err)
        return err;

    err = ciot_storage_load_data(&settings->ntp, sizeof(settings->ntp), CIOT_SETTINGS_NTP_FILENAME);
    if(err)
        return err;

    return err;
}

ciot_err_t ciot_settings_clear(void)
{
    ciot_err_t err = CIOT_ERR_OK;

    err = ciot_storage_remove_data(CIOT_SETTINGS_WIFI_FILENAME);
    if(err)
        return err;
    
    err = ciot_storage_remove_data(CIOT_SETTINGS_NTP_FILENAME);
    if(err)
        return err;

    return err;
}
