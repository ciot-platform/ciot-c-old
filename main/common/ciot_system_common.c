/**
 * @file ciot_system_common.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_system.h"
#include "ciot_settings.h"

typedef struct ciot_system
{
    ciot_system_config_t config;
    uint32_t err_code;
    uint32_t status_code;
} ciot_system_t;

static ciot_system_t this;

ciot_err_t ciot_system_set_config(ciot_system_config_t *conf)
{
    return CIOT_ERR_NOTHING_TO_DO;
}

ciot_err_t ciot_system_get_config(ciot_system_config_t *config)
{
    *config = this.config;
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
        printf("CIOT_SYSTEM_REQUEST_RESTART\n");
        return ciot_system_reset();
    case CIOT_SYSTEM_REQUEST_SAVE_SETTINGS:
        printf("CIOT_SYSTEM_REQUEST_SAVE_SETTINGS\n");
        return ciot_settings_save();
    case CIOT_SYSTEM_REQUEST_CLEAR_SETTINGS:
        printf("CIOT_SYSTEM_REQUEST_CLEAR_SETTINGS\n");
        return ciot_settings_clear();
    default:
        return CIOT_ERR_INVALID_REQUEST;
    }
}
