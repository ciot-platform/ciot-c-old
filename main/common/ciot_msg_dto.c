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

ciot_err_t ciot_msg_from_json(CJSON_PARAMETERS(ciot_msg_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(type);
    CJSON_GET_UNION(data, ciot_msg_data_from_json, type);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_data_from_json(CJSON_PARAMETERS(ciot_msg_data_t), ciot_msg_type_t type)
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_OBJ_IN_UNION(request, ciot_msg_request_from_json, type, CIOT_MSG_TYPE_REQUEST);
    CJSON_GET_OBJ_IN_UNION(config, ciot_msg_config_from_json, type, CIOT_MSG_TYPE_CONFIG);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_request_from_json(CJSON_PARAMETERS(ciot_msg_request_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(request);
    CJSON_GET_NUMBER(interface);
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
    CJSON_GET_OBJ_IN_UNION(wifi, ciot_wifi_config_from_json, interface, CIOT_MSG_IF_WIFI);
    CJSON_GET_OBJ_IN_UNION(system, ciot_system_config_from_json, interface, CIOT_MSG_IF_SYSTEM);
    CJSON_GET_OBJ_IN_UNION(ntp, ciot_ntp_config_from_json, interface, CIOT_MSG_IF_NTP);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_response_to_json(CJSON_PARAMETERS(ciot_msg_response_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(err_code);
    CJSON_ADD_NUMBER(request);
    CJSON_ADD_UNION_TO_ROOT(data, ciot_msg_response_data_to_json, request);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_msg_response_data_to_json(CJSON_PARAMETERS(ciot_msg_response_data_t), ciot_msg_request_type_t request)
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ_IN_UNION(config, ciot_msg_config_to_json, request, CIOT_MSG_REQUEST_GET_CONFIG);
    CJSON_ADD_OBJ_IN_UNION(info, ciot_msg_info_to_json, request, CIOT_MSG_REQUEST_GET_INFO);
    CJSON_ADD_OBJ_IN_UNION(status, ciot_msg_status_to_json, request, CIOT_MSG_REQUEST_GET_STATUS);
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
    CJSON_ADD_OBJ_IN_UNION(wifi, ciot_wifi_info_to_json, interface, CIOT_MSG_IF_WIFI);
    CJSON_ADD_OBJ_IN_UNION(system, ciot_system_info_to_json, interface, CIOT_MSG_IF_SYSTEM);
    CJSON_ADD_OBJ_IN_UNION(ntp, ciot_ntp_info_to_json, interface, CIOT_MSG_IF_NTP);
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
    CJSON_ADD_OBJ_IN_UNION(wifi, ciot_wifi_status_to_json, interface, CIOT_MSG_IF_WIFI);
    CJSON_ADD_OBJ_IN_UNION(system, ciot_system_status_to_json, interface, CIOT_MSG_IF_SYSTEM);
    CJSON_ADD_OBJ_IN_UNION(ntp, ciot_ntp_status_to_json, interface, CIOT_MSG_IF_NTP);
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
    CJSON_ADD_OBJ_IN_UNION(wifi, ciot_wifi_config_to_json, interface, CIOT_MSG_IF_WIFI);
    CJSON_ADD_OBJ_IN_UNION(system, ciot_system_config_to_json, interface, CIOT_MSG_IF_SYSTEM);
    CJSON_ADD_OBJ_IN_UNION(ntp, ciot_ntp_config_to_json, interface, CIOT_MSG_IF_NTP);
    return CIOT_ERR_OK;
}
