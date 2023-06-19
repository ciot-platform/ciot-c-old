/**
 * @file ciot_default_config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_DEFAULT_CONFIG__H__
#define __CIOT_DEFAULT_CONFIG__H__

#include "ciot_system.h"

#define CIOT_CONFIG_DEVICE_TYPE CIOT_SYSTEM_DEVICE_TYPE_GENERIC

#if defined(_WIN32) || defined(_WIN64)
    #define CIOT_CONFIG_OS CIOT_SYSTEM_OS_WINDOWS
#elif defined(__linux__)
    #define CIOT_CONFIG_OS CIOT_SYSTEM_OS_LINUX
#elif defined(__APPLE__)
    #define CIOT_CONFIG_OS CIOT_SYSTEM_OS_MAC
#else
    #define CIOT_CONFIG_OS CIOT_SYSTEM_OS_UNKNOWN
#endif

#define CIOT_CONFIG_BOARD "Generic IoT Board"
#define CIOT_CONFIG_MCU "Unknown"

#define CIOT_CONFIG_FEATURES          \
    (1 << CIOT_SYSTEM_FEATURE_UART) | \
    (1 << CIOT_SYSTEM_FEATURE_UART) | \
    (1 << CIOT_SYSTEM_FEATURE_USB) | \
    (1 << CIOT_SYSTEM_FEATURE_ETHERNET) | \
    (1 << CIOT_SYSTEM_FEATURE_WIFI) | \
    (1 << CIOT_SYSTEM_FEATURE_BLE) | \
    (1 << CIOT_SYSTEM_FEATURE_HARDDISK) | \
    (1 << CIOT_SYSTEM_FEATURE_DISPLAY) | \
    (1 << CIOT_SYSTEM_FEATURE_FLASH)

#endif  //!__CIOT_DEFAULT_CONFIG__H__