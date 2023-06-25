/**
 * @file ciot_config.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_CONFIG__H__
#define __CIOT_CONFIG__H__

#if __has_include("ciot_custom_config.h")
#include "ciot_custom_config.h"
#else
#warning "Using default configs. Create a ciot_custom_config.h file and include it on your project to define your custom configurations."
#include "ciot_default_config.h"

#define CIOT_CONFIG_WIFI_AP_FILENAME "wifi_ap.dat"
#define CIOT_CONFIG_WIFI_STA_FILENAME "wifi_sta.dat"
#define CIOT_CONFIG_NTP_FILENAME "ntp.dat"

#define CIOT_CONFIG_VERSION { 0, 0, 0, 1 }
#define CIOT_CONFIG_DATE { 19, 06, 23 }
#define CIOT_CONFIG_DEFAULT_TIMEZONE "<-03>3"

#endif

#endif //!__CIOT_CONFIG__H__