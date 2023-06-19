/**
 * @file ciot_wifi.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_WIFI__H__
#define __CIOT_WIFI__H__

#include "ciot_err.h"
#include "ciot_tcp.h"
#include "ciot_json.h"

typedef enum ciot_wifi_type
{
    CIOT_WIFI_TYPE_STA,
    CIOT_WIFI_TYPE_AP,
} ciot_wifi_type_t;

typedef struct ciot_wifi_config
{
    char ssid[32];
    char password[64];
    ciot_tcp_config_t tcp;
} ciot_wifi_config_t;

typedef struct ciot_wifi_info
{
    ciot_tcp_info_t tcp;
} ciot_wifi_info_t;

typedef struct ciot_wifi_status
{
    ciot_tcp_status_t tcp;
} ciot_wifi_status_t;

ciot_err_t ciot_wifi_set_config(ciot_wifi_type_t type, ciot_wifi_config_t *conf);
ciot_err_t ciot_wifi_get_config(ciot_wifi_type_t type, ciot_wifi_config_t *config);
ciot_err_t ciot_wifi_get_status(ciot_wifi_type_t type, ciot_wifi_status_t *status);
ciot_err_t ciot_wifi_get_info(ciot_wifi_type_t type, ciot_wifi_info_t *info);
ciot_err_t ciot_wifi_wait_connection(int timeout);

ciot_err_t ciot_wifi_config_from_json(CJSON_PARAMETERS(ciot_wifi_config_t));
ciot_err_t ciot_wifi_config_to_json(CJSON_PARAMETERS(ciot_wifi_config_t));
ciot_err_t ciot_wifi_info_from_json(CJSON_PARAMETERS(ciot_wifi_info_t));
ciot_err_t ciot_wifi_info_to_json(CJSON_PARAMETERS(ciot_wifi_info_t));
ciot_err_t ciot_wifi_status_from_json(CJSON_PARAMETERS(ciot_wifi_status_t));
ciot_err_t ciot_wifi_status_to_json(CJSON_PARAMETERS(ciot_wifi_status_t));

#endif  //!__CIOT_WIFI__H__