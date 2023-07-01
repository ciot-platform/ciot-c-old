/**
 * @file ciot_custom_config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_CUSTOM_CONFIG__H__
#define __CIOT_CUSTOM_CONFIG__H__

#if defined(_WIN32)
#include "ciot_config_win.h"
#elif defined(__linux__)
#include "ciot_config_linux.h"
#else
#include "ciot_config_idf.h"
#endif  // ESP_PLATFORM

#define CIOT_CONFIG_WIFI_AP_MODE CIOT_WIFI_IF_AP
#define CIOT_CONFIG_WIFI_AP_SSID "CIOT AP"
#define CIOT_CONFIG_WIFI_AP_PASSWORD "administrator"
#define CIOT_CONFIG_HTTP_SERVER_PORT 8000
#define CIOT_CONFIG_HTTP_SERVER_ENDPOINT "/v1/ciot"

#endif  //!__CIOT_CUSTOM_CONFIG__H__