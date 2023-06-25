/**
 * @file ciot_message.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_MSG__H__
#define __CIOT_MSG__H__

#include <stdbool.h>

#include "ciot_json.h"
#include "ciot_wifi.h"
#include "ciot_system.h"
#include "ciot_ntp.h"

typedef enum ciot_msg_type
{
    CIOT_MSG_TYPE_UNKNOWN,
    CIOT_MSG_TYPE_REQUEST,
    CIOT_MSG_TYPE_CONFIG,
    CIOT_MSG_TYPE_RESPONSE,
} ciot_msg_type_t;

typedef enum ciot_msg_request_type
{
    CIOT_MSG_REQUEST_NONE,
    CIOT_MSG_REQUEST_GET_CONFIG,
    CIOT_MSG_REQUEST_GET_INFO,
    CIOT_MSG_REQUEST_GET_STATUS,
    CIOT_MSG_REQUEST_CUSTOM,
} ciot_msg_request_type_t;

typedef enum ciot_msg_interface
{
    CIOT_MSG_IF_UNKNOWN,
    CIOT_MSG_IF_WIFI,
    CIOT_MSG_IF_SYSTEM = 64,
    CIOT_MSG_IF_NTP,
} ciot_msg_interface_t;

typedef struct ciot_msg_request
{
    ciot_msg_interface_t interface;
    ciot_msg_request_type_t request;
} ciot_msg_request_t;

typedef union ciot_msg_config_data
{
    ciot_wifi_config_t wifi;
    ciot_system_config_t system;
    ciot_ntp_config_t ntp;
} ciot_msg_config_data_t;

typedef struct ciot_msg_config
{
    ciot_msg_interface_t interface;
    ciot_msg_config_data_t data;
} ciot_msg_config_t;

typedef union ciot_msg_info_data
{
    ciot_wifi_info_t wifi;
    ciot_system_info_t system;
    ciot_ntp_info_t ntp;
} ciot_msg_info_data_t;

typedef struct ciot_msg_info
{
    ciot_msg_interface_t interface;
    ciot_msg_info_data_t data;
} ciot_msg_info_t;

typedef union ciot_msg_status_data
{
    ciot_wifi_status_t wifi;
    ciot_system_status_t system;
    ciot_ntp_status_t ntp;
} ciot_msg_status_data_t;

typedef struct ciot_msg_status
{
    ciot_msg_interface_t interface;
    ciot_msg_status_data_t data;
} ciot_msg_status_t;

typedef union ciot_msg_response_data
{
    ciot_msg_config_t config;
    ciot_msg_info_t info;
    ciot_msg_status_t status;
} ciot_msg_response_data_t;

typedef struct ciot_msg_response
{
    ciot_err_t err_code;
    ciot_msg_request_type_t request;
    ciot_msg_response_data_t data;
} ciot_msg_response_t;

typedef union ciot_msg_data
{
    ciot_msg_request_t request;
    ciot_msg_config_t config;
    ciot_msg_response_t response;
} ciot_msg_data_t;

typedef struct ciot_msg
{
    ciot_msg_type_t type;
    ciot_msg_data_t data;
} ciot_msg_t;

ciot_err_t ciot_msg_from_json(CJSON_PARAMETERS(ciot_msg_t));
ciot_err_t ciot_msg_data_from_json(CJSON_PARAMETERS(ciot_msg_data_t), ciot_msg_type_t type);
ciot_err_t ciot_msg_request_from_json(CJSON_PARAMETERS(ciot_msg_request_t));

ciot_err_t ciot_msg_config_from_json(CJSON_PARAMETERS(ciot_msg_config_t));
ciot_err_t ciot_msg_config_data_from_json(CJSON_PARAMETERS(ciot_msg_config_data_t), ciot_msg_interface_t interface);

ciot_err_t ciot_msg_response_to_json(CJSON_PARAMETERS(ciot_msg_response_t));
ciot_err_t ciot_msg_response_data_to_json(CJSON_PARAMETERS(ciot_msg_response_data_t), ciot_msg_request_type_t request);

ciot_err_t ciot_msg_info_to_json(CJSON_PARAMETERS(ciot_msg_info_t));
ciot_err_t ciot_msg_info_data_to_json(CJSON_PARAMETERS(ciot_msg_info_data_t), ciot_msg_interface_t interface);

ciot_err_t ciot_msg_status_to_json(CJSON_PARAMETERS(ciot_msg_status_t));
ciot_err_t ciot_msg_status_data_to_json(CJSON_PARAMETERS(ciot_msg_status_data_t), ciot_msg_interface_t interface);

ciot_err_t ciot_msg_config_to_json(CJSON_PARAMETERS(ciot_msg_config_t));
ciot_err_t ciot_msg_config_data_to_json(CJSON_PARAMETERS(ciot_msg_config_data_t), ciot_msg_interface_t interface);

#endif  //!__CIOT_MSG__H__