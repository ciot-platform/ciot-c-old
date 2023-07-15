/**
 * @file ciot_msg.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_msg_dto.h"
#include "ciot_config.h"

ciot_err_t ciot_msg_from_json(CJSON_PARAMETERS(ciot_msg_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(msg);
    CJSON_GET_UNION(data, ciot_msg_data_from_json, msg);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_data_from_json(CJSON_PARAMETERS(ciot_msg_data_t), ciot_msg_type_t msg)
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_OBJ_UNION_CHILD(request, ciot_msg_request_from_json, msg, CIOT_MSG_TYPE_REQUEST);
    CJSON_GET_OBJ_UNION_CHILD(config, ciot_msg_config_from_json, msg, CIOT_MSG_TYPE_CONFIG);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_request_from_json(CJSON_PARAMETERS(ciot_msg_request_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(request);
    CJSON_GET_NUMBER(interface);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_request_to_json(CJSON_PARAMETERS(ciot_msg_request_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(request);
    CJSON_ADD_NUMBER(interface);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_config_from_json(CJSON_PARAMETERS(ciot_msg_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(interface);
    CJSON_GET_UNION(data, ciot_msg_config_data_from_json, interface);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_config_data_from_json(CJSON_PARAMETERS(ciot_msg_config_data_t), ciot_msg_interface_t interface)
{
    CJSON_CHECK_PARAMETERS();
#if CIOT_CONFIG_FEATURE_WIFI
    CJSON_GET_OBJ_UNION_CHILD(wifi, ciot_wifi_config_from_json, interface, CIOT_MSG_IF_WIFI);
#endif
#if CIOT_CONFIG_FEATURE_SYS
    CJSON_GET_OBJ_UNION_CHILD(system, ciot_system_config_from_json, interface, CIOT_MSG_IF_SYSTEM);
#endif
#if CIOT_CONFIG_FEATURE_NTP
    CJSON_GET_OBJ_UNION_CHILD(ntp, ciot_ntp_config_from_json, interface, CIOT_MSG_IF_NTP);
#endif
#if CIOT_CONFIG_FEATURE_MQTT
    CJSON_GET_OBJ_UNION_CHILD(mqtt, ciot_mqtt_config_from_json, interface, CIOT_MSG_IF_MQTT);
#endif
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_response_to_json(CJSON_PARAMETERS(ciot_msg_response_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(msg);
    CJSON_ADD_OBJ_TO_ROOT(request, ciot_msg_request_to_json);
    if (ptr->err != CIOT_ERR_OK)
    {
#if CIOT_CONFIG_API_ERROR_MESSAGES
        const char *err_msg = ciot_err_to_message(ptr->err);
        cJSON_AddStringToObject(json, "err_msg", err_msg);
#endif
        CJSON_ADD_NUMBER(err);
    }
    if (ptr->err != CIOT_ERR_FEATURE_NOT_SUPPORTED)
    {
        CJSON_ADD_UNION_TO_ROOT(data, ciot_msg_response_data_to_json, request);
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_response_data_to_json(CJSON_PARAMETERS(ciot_msg_response_data_t), ciot_msg_request_t request)
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_UNION_CHILD_OF_UNION(config, ciot_msg_config_data_to_json, request.request, CIOT_MSG_REQUEST_GET_CONFIG, request.interface);
    CJSON_ADD_UNION_CHILD_OF_UNION(info, ciot_msg_info_data_to_json, request.request, CIOT_MSG_REQUEST_GET_INFO, request.interface);
    CJSON_ADD_UNION_CHILD_OF_UNION(status, ciot_msg_status_data_to_json, request.request, CIOT_MSG_REQUEST_GET_STATUS, request.interface);
    switch (request.interface)
    {
#if CIOT_CONFIG_FEATURE_WIFI
    case CIOT_MSG_IF_WIFI:
        CJSON_ADD_OBJ_UNION_CHILD(wifi, ciot_wifi_scan_result_to_json, (int)request.request, (int)CIOT_WIFI_REQUEST_SCAN);
        break;
#endif
    default:
        break;
    }
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_info_to_json(CJSON_PARAMETERS(ciot_msg_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(interface);
    CJSON_ADD_UNION(data, ciot_msg_info_data_to_json, interface);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_info_data_to_json(CJSON_PARAMETERS(ciot_msg_info_data_t), ciot_msg_interface_t interface)
{
    CJSON_CHECK_PARAMETERS();
#if CIOT_CONFIG_FEATURE_WIFI
    CJSON_ADD_OBJ_UNION_CHILD(wifi, ciot_wifi_info_to_json, interface, CIOT_MSG_IF_WIFI);
#endif
#if CIOT_CONFIG_FEATURE_SYS
    CJSON_ADD_OBJ_UNION_CHILD(system, ciot_system_info_to_json, interface, CIOT_MSG_IF_SYSTEM);
#endif
#if CIOT_CONFIG_FEATURE_NTP
    CJSON_ADD_OBJ_UNION_CHILD(ntp, ciot_ntp_info_to_json, interface, CIOT_MSG_IF_NTP);
#endif
#if CIOT_CONFIG_FEATURE_MQTT
    CJSON_ADD_OBJ_UNION_CHILD(mqtt, ciot_mqtt_info_to_json, interface, CIOT_MSG_IF_MQTT);
#endif
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_status_to_json(CJSON_PARAMETERS(ciot_msg_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(interface);
    CJSON_ADD_UNION_TO_ROOT(data, ciot_msg_status_data_to_json, interface);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_status_data_to_json(CJSON_PARAMETERS(ciot_msg_status_data_t), ciot_msg_interface_t interface)
{
    CJSON_CHECK_PARAMETERS();
#if CIOT_CONFIG_FEATURE_WIFI
    CJSON_ADD_OBJ_UNION_CHILD(wifi, ciot_wifi_status_to_json, interface, CIOT_MSG_IF_WIFI);
#endif
#if CIOT_CONFIG_FEATURE_SYS
    CJSON_ADD_OBJ_UNION_CHILD(system, ciot_system_status_to_json, interface, CIOT_MSG_IF_SYSTEM);
#endif
#if CIOT_CONFIG_FEATURE_NTP
    CJSON_ADD_OBJ_UNION_CHILD(ntp, ciot_ntp_status_to_json, interface, CIOT_MSG_IF_NTP);
#endif
#if CIOT_CONFIG_FEATURE_MQTT
    CJSON_ADD_OBJ_UNION_CHILD(mqtt, ciot_mqtt_status_to_json, interface, CIOT_MSG_IF_MQTT);
#endif
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_config_to_json(CJSON_PARAMETERS(ciot_msg_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(interface);
    CJSON_ADD_UNION_TO_ROOT(data, ciot_msg_config_data_to_json, interface);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_config_data_to_json(CJSON_PARAMETERS(ciot_msg_config_data_t), ciot_msg_interface_t interface)
{
    CJSON_CHECK_PARAMETERS();
#if CIOT_CONFIG_FEATURE_WIFI
    CJSON_ADD_OBJ_UNION_CHILD(wifi, ciot_wifi_config_to_json, interface, CIOT_MSG_IF_WIFI);
#endif
#if CIOT_CONFIG_FEATURE_SYS
    CJSON_ADD_OBJ_UNION_CHILD(system, ciot_system_config_to_json, interface, CIOT_MSG_IF_SYSTEM);
#endif
#if CIOT_CONFIG_FEATURE_NTP
    CJSON_ADD_OBJ_UNION_CHILD(ntp, ciot_ntp_config_to_json, interface, CIOT_MSG_IF_NTP);
#endif
#if CIOT_CONFIG_FEATURE_MQTT
    CJSON_ADD_OBJ_UNION_CHILD(mqtt, ciot_mqtt_config_to_json, interface, CIOT_MSG_IF_MQTT);
#endif
    return CIOT_ERR_OK;
}
