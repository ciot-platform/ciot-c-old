/**
 * @file ciot_wifi_dto.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_wifi.h"

ciot_err_t ciot_wifi_config_from_json(CJSON_PARAMETERS(ciot_wifi_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(mode);
    CJSON_GET_CHAR_ARRAY(ssid);
    CJSON_GET_CHAR_ARRAY(password);
    CJSON_GET_NUMBER(timeout);
    CJSON_GET_OBJ_FROM_ROOT(tcp, ciot_tcp_config_from_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_config_to_json(CJSON_PARAMETERS(ciot_wifi_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(mode);
    CJSON_ADD_CHAR_ARRAY(ssid);
    CJSON_ADD_CHAR_ARRAY(password);
    CJSON_ADD_NUMBER(timeout);
    CJSON_ADD_OBJ_TO_ROOT(tcp, ciot_tcp_config_to_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_info_from_json(CJSON_PARAMETERS(ciot_wifi_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_OBJ_FROM_ROOT(tcp, ciot_tcp_info_from_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_info_to_json(CJSON_PARAMETERS(ciot_wifi_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ_TO_ROOT(tcp, ciot_tcp_info_to_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_status_from_json(CJSON_PARAMETERS(ciot_wifi_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_OBJ_FROM_ROOT(tcp, ciot_tcp_status_from_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_status_to_json(CJSON_PARAMETERS(ciot_wifi_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ_TO_ROOT(tcp, ciot_tcp_status_to_json);
    return CIOT_ERR_OK;
}
