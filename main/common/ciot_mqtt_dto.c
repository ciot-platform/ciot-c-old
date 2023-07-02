/**
 * @file ciot_mqtt_dto.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_mqtt.h"

ciot_err_t ciot_mqtt_config_from_json(CJSON_PARAMETERS(ciot_mqtt_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_CHAR_ARRAY(host);
    CJSON_GET_NUMBER(port);
    CJSON_GET_CHAR_ARRAY(username);
    CJSON_GET_CHAR_ARRAY(password);
    CJSON_GET_NUMBER(qos);
    CJSON_GET_NUMBER(transport);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_config_to_json(CJSON_PARAMETERS(ciot_mqtt_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_CHAR_ARRAY(host);
    CJSON_ADD_NUMBER(port);
    CJSON_ADD_CHAR_ARRAY(username);
    CJSON_ADD_CHAR_ARRAY(password);
    CJSON_ADD_NUMBER(qos);
    CJSON_ADD_NUMBER(transport);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_info_from_json(CJSON_PARAMETERS(ciot_mqtt_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(version);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_info_to_json(CJSON_PARAMETERS(ciot_mqtt_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(version);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_status_from_json(CJSON_PARAMETERS(ciot_mqtt_status_t))
{
    CJSON_CHECK_PARAMETERS();
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_status_to_json(CJSON_PARAMETERS(ciot_mqtt_status_t))
{
    CJSON_CHECK_PARAMETERS();
    return CIOT_ERR_OK;
}
