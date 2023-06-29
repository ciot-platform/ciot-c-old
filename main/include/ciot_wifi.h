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

typedef enum ciot_wifi_interface
{
    CIOT_WIFI_IF_STA,
    CIOT_WIFI_IF_AP,
} ciot_wifi_interface_t;

typedef enum ciot_wifi_scan_state
{
    CIOT_WIFI_SCAN_STATE_ERROR=-1,
    CIOT_WIFI_SCAN_STATE_IDLE,
    CIOT_WIFI_SCAN_STATE_SCANNING,
    CIOT_WIFI_SCAN_STATE_SCANNED,
} ciot_wifi_scan_state_t;

typedef enum ciot_wifi_request
{
    CIOT_WIFI_REQUEST_SCAN=4,
} ciot_wifi_request_t;

typedef struct ciot_wifi_config
{
    char ssid[32];
    char password[64];
    int timeout;
    ciot_wifi_interface_t mode;
    ciot_tcp_config_t tcp;
} ciot_wifi_config_t;

typedef struct ciot_wifi_info
{
    ciot_tcp_info_t tcp;
} ciot_wifi_info_t;

typedef struct ciot_wifi_ap_info
{
    uint8_t bssid[6];
    char ssid[32];
    int8_t rssi;
    uint8_t authmode;
} ciot_wifi_ap_info_t;

typedef struct ciot_wifi_scan_result
{
    ciot_wifi_scan_state_t state;
    size_t size;
    ciot_wifi_ap_info_t result[10];
} ciot_wifi_scan_result_t;

typedef struct ciot_wifi_ap_status
{
    ciot_tcp_status_t tcp;
} ciot_wifi_ap_status_t;

typedef struct ciot_wifi_sta_status
{
    ciot_tcp_status_t tcp;
    ciot_wifi_ap_info_t ap;
    ciot_wifi_scan_state_t scan;
} ciot_wifi_sta_status_t;

typedef union ciot_wifi_status_data
{
    ciot_tcp_status_t tcp;
    ciot_wifi_sta_status_t sta;
    ciot_wifi_ap_status_t ap;
} ciot_wifi_status_data_t;

typedef struct ciot_wifi_status
{
    ciot_wifi_interface_t interface;
    ciot_wifi_status_data_t data;
} ciot_wifi_status_t;

ciot_err_t ciot_wifi_set_config(ciot_wifi_config_t *conf);
ciot_err_t ciot_wifi_get_config(ciot_wifi_interface_t type, ciot_wifi_config_t *config);
ciot_err_t ciot_wifi_get_status(ciot_wifi_interface_t type, ciot_wifi_status_t *status);
ciot_err_t ciot_wifi_get_info(ciot_wifi_interface_t type, ciot_wifi_info_t *info);
ciot_err_t ciot_wifi_process_request(ciot_wifi_request_t request, ciot_wifi_scan_result_t *scan_result);

ciot_err_t ciot_wifi_config_from_json(CJSON_PARAMETERS(ciot_wifi_config_t));
ciot_err_t ciot_wifi_config_to_json(CJSON_PARAMETERS(ciot_wifi_config_t));
ciot_err_t ciot_wifi_info_from_json(CJSON_PARAMETERS(ciot_wifi_info_t));
ciot_err_t ciot_wifi_info_to_json(CJSON_PARAMETERS(ciot_wifi_info_t));
ciot_err_t ciot_wifi_status_from_json(CJSON_PARAMETERS(ciot_wifi_status_t));
ciot_err_t ciot_wifi_status_to_json(CJSON_PARAMETERS(ciot_wifi_status_t));
ciot_err_t ciot_wifi_scan_result_to_json(CJSON_PARAMETERS(ciot_wifi_scan_result_t));

#endif  //!__CIOT_WIFI__H__