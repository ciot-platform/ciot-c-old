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

#define CIOT_CONFIG_WIFI_AP_FILENAME "wifi_ap.dat"
#define CIOT_CONFIG_WIFI_STA_FILENAME "wifi_sta.dat"
#define CIOT_CONFIG_NTP_FILENAME "ntp.dat"
#define CIOT_CONFIG_MQTT_FILENAME "mqtt.dat"

#define CIOT_CONFIG_WIFI_AP_MODE CIOT_WIFI_IF_AP
#define CIOT_CONFIG_WIFI_AP_SSID "CIOT AP"
#define CIOT_CONFIG_WIFI_AP_PASSWORD "administrator"
#define CIOT_CONFIG_HTTP_SERVER_PORT 8000
#define CIOT_CONFIG_HTTP_SERVER_ENDPOINT "/v1/ciot"

#define CIOT_CONFIG_VERSION { 0, 2, 0, 0 }
#define CIOT_CONFIG_DATE { 23, 07, 26 }
#define CIOT_CONFIG_DEFAULT_TIMEZONE "<-03>3"

#define CIOT_CONFIG_API_ERROR_MESSAGES 1
#define CIOT_CONFIG_CONSOLE_ERROR_MESSAGES 1

#endif  //!__CIOT_CUSTOM_CONFIG__H__