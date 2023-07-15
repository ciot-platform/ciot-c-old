/**
 * @file ciot_system_dto.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_system.h"
#include "ciot_app.h"

typedef struct ciot_system
{
    ciot_system_config_t config;
    uint32_t err_code;
    uint32_t status_code;
    ciot_settings_t settings;
} ciot_system_t;

static ciot_system_t this;

ciot_err_t ciot_system_config_from_json(CJSON_PARAMETERS(ciot_system_config_t))
{
    CJSON_CHECK_PARAMETERS();
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_config_to_json(CJSON_PARAMETERS(ciot_system_config_t))
{
    CJSON_CHECK_PARAMETERS();
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_info_from_json(CJSON_PARAMETERS(ciot_system_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(device);
    CJSON_GET_NUMBER(os);
    CJSON_GET_NUMBER_ARRAY(version);
    CJSON_GET_CHAR_ARRAY(date);
    CJSON_GET_CHAR_ARRAY(board);
    CJSON_GET_CHAR_ARRAY(mcu);
    CJSON_GET_NUMBER(storage);
    CJSON_GET_NUMBER(features);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_info_to_json(CJSON_PARAMETERS(ciot_system_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(device);
    CJSON_ADD_NUMBER(os);
    CJSON_ADD_NUMBER_ARRAY(version);
    CJSON_ADD_NUMBER_ARRAY(date);
    CJSON_ADD_CHAR_ARRAY(board);
    CJSON_ADD_CHAR_ARRAY(mcu);
    CJSON_ADD_NUMBER(storage);
    CJSON_ADD_NUMBER(features);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_status_from_json(CJSON_PARAMETERS(ciot_system_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(time);
    CJSON_GET_NUMBER(memory);
    CJSON_GET_NUMBER(sys_err);
    CJSON_GET_NUMBER(sys_status);
    CJSON_GET_NUMBER(lifetime);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_status_to_json(CJSON_PARAMETERS(ciot_system_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(time);
    CJSON_ADD_NUMBER(memory);
    CJSON_ADD_NUMBER(sys_err);
    CJSON_ADD_NUMBER(sys_status);
    CJSON_ADD_NUMBER(lifetime);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_set_config(ciot_system_config_t *conf)
{
    return CIOT_ERR_NOTHING_TO_DO;
}

ciot_err_t ciot_system_get_config(ciot_system_config_t *config)
{
    *config = this.config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_set_err_code(uint32_t code)
{
    this.err_code = code;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_set_status_code(uint32_t code)
{
    this.status_code = code;
    return CIOT_ERR_OK;
}

uint32_t ciot_system_get_err_code(void)
{
    return this.err_code;
}

uint32_t ciot_system_get_status_code()
{
    return this.status_code;
}

ciot_err_t ciot_system_process_request(ciot_system_request_t request)
{
    switch (request)
    {
    case CIOT_SYSTEM_REQUEST_RESTART:
        printf("CIOT_SYSTEM_REQUEST_RESTART\n");
        return ciot_system_reset();
    case CIOT_SYSTEM_REQUEST_SAVE_SETTINGS:
    {
        printf("CIOT_SYSTEM_REQUEST_SAVE_SETTINGS\n");
        ciot_settings_t settings;
        ciot_app_get_settings(&settings);
        return ciot_settings_save(&settings);
    }
    case CIOT_SYSTEM_REQUEST_CLEAR_SETTINGS:
        printf("CIOT_SYSTEM_REQUEST_CLEAR_SETTINGS\n");
        return ciot_settings_clear();
    default:
        return CIOT_ERR_INVALID_REQUEST;
    }
}