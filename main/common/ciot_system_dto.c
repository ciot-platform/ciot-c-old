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
    CJSON_GET_NUMBER(err);
    CJSON_GET_NUMBER(status);
    CJSON_GET_NUMBER(lifetime);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_status_to_json(CJSON_PARAMETERS(ciot_system_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(time);
    CJSON_ADD_NUMBER(memory);
    CJSON_ADD_NUMBER(err);
    CJSON_ADD_NUMBER(status);
    CJSON_ADD_NUMBER(lifetime);
    return CIOT_ERR_OK;
}
