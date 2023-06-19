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

#include "ciot_system.h"

#define CIOT_CONFIG_DEVICE_TYPE CIOT_SYSTEM_DEVICE_TYPE_GENERIC
#define CIOT_CONFIG_OS CIOT_SYSTEM_OS_FREERTOS
#define CIOT_CONFIG_VERSION { 0, 0, 0, 1 }
#define CIOT_CONFIG_DATE { 18, 06, 23 }
#define CIOT_CONFIG_BOARD "ESP32 Devkit"
#define CIOT_CONFIG_MCU "ESP32"

#define CIOT_CONFIG_FEATURES          \
    (1 << CIOT_SYSTEM_FEATURE_I2C) |  \
    (1 << CIOT_SYSTEM_FEATURE_SPI) |  \
    (1 << CIOT_SYSTEM_FEATURE_UART) | \
    (1 << CIOT_SYSTEM_FEATURE_WIFI) | \
    (1 << CIOT_SYSTEM_FEATURE_BLE) |  \
    (1 << CIOT_SYSTEM_FEATURE_FLASH)

#endif  //!__CIOT_CONFIG__H__