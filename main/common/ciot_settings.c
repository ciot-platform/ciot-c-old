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

#define CIOT_SETTINGS_FILENAME "ciot.dat"

ciot_err_t ciot_settings_save(void)
{
    ciot_settings_t settings;
    ciot_wifi_get_config(CIOT_WIFI_TYPE_STA, &settings.wifi);
    ciot_ntp_get_config(&settings.ntp);
    return ciot_storage_save_data(&settings, sizeof(settings), CIOT_SETTINGS_FILENAME);
}

ciot_err_t ciot_settings_load(ciot_settings_t *settings)
{
    return ciot_storage_load_data(settings, sizeof(*settings), CIOT_SETTINGS_FILENAME);
}

ciot_err_t ciot_settings_clear(void)
{
    return ciot_storage_remove_data(CIOT_SETTINGS_FILENAME);
}
