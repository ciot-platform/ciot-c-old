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

static ciot_err_t ciot_wifi_status_data_to_json(CJSON_PARAMETERS(ciot_wifi_status_data_t), ciot_wifi_interface_t interface);
static ciot_err_t ciot_wifi_ap_status_to_json(CJSON_PARAMETERS(ciot_wifi_ap_status_t));
static ciot_err_t ciot_wifi_sta_status_to_json(CJSON_PARAMETERS(ciot_wifi_sta_status_t));
static ciot_err_t ciot_wifi_ap_info_to_json(CJSON_PARAMETERS(ciot_wifi_ap_info_t));

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
    CJSON_GET_BOOLEAN(available);
    CJSON_GET_OBJ_FROM_ROOT(tcp, ciot_tcp_info_from_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_info_to_json(CJSON_PARAMETERS(ciot_wifi_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_BOOLEAN(available);
    CJSON_ADD_OBJ_TO_ROOT(tcp, ciot_tcp_info_to_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_status_to_json(CJSON_PARAMETERS(ciot_wifi_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_UNION_TO_ROOT(data, ciot_wifi_status_data_to_json, interface);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_status_data_to_json(CJSON_PARAMETERS(ciot_wifi_status_data_t), ciot_wifi_interface_t interface)
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ_CHILD_OF_UNION(sta, ciot_wifi_sta_status_to_json, interface, CIOT_WIFI_IF_STA);
    CJSON_ADD_OBJ_CHILD_OF_UNION(ap, ciot_wifi_ap_status_to_json, interface, CIOT_WIFI_IF_AP);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_ap_status_to_json(CJSON_PARAMETERS(ciot_wifi_ap_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ_TO_ROOT(tcp, ciot_tcp_status_to_json);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_sta_status_to_json(CJSON_PARAMETERS(ciot_wifi_sta_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ(sta, ciot_wifi_ap_info_to_json);
    CJSON_ADD_OBJ(tcp, ciot_tcp_status_to_json);
    CJSON_ADD_NUMBER(scan);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_scan_result_to_json(CJSON_PARAMETERS(ciot_wifi_scan_result_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(state);
    CJSON_ADD_OBJ_ARRAY(result, ciot_wifi_ap_info_to_json, ptr->size);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_wifi_ap_info_to_json(CJSON_PARAMETERS(ciot_wifi_ap_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_CHAR_ARRAY(ssid);
    CJSON_ADD_NUMBER(rssi);
    CJSON_ADD_NUMBER(authmode);
    CJSON_ADD_NUMBER_ARRAY(bssid);
    return CIOT_ERR_OK;
}
