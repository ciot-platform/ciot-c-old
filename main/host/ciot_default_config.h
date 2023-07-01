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

#define CIOT_CONFIG_API_ERROR_MESSAGES 0
#define CIOT_CONFIG_CONSOLE_ERROR_MESSAGES 0

#define CIOT_CONFIG_FEATURE_I2C 0
#define CIOT_CONFIG_FEATURE_SPI 0
#define CIOT_CONFIG_FEATURE_UART 0
#define CIOT_CONFIG_FEATURE_USB 0
#define CIOT_CONFIG_FEATURE_ETHERNET 0
#define CIOT_CONFIG_FEATURE_WIFI 0
#define CIOT_CONFIG_FEATURE_BLE 0
#define CIOT_CONFIG_FEATURE_ZIGBEE 0
#define CIOT_CONFIG_FEATURE_LORA 0
#define CIOT_CONFIG_FEATURE_RS232 0
#define CIOT_CONFIG_FEATURE_RS485 0
#define CIOT_CONFIG_FEATURE_CAN 0
#define CIOT_CONFIG_FEATURE_GPIO 0
#define CIOT_CONFIG_FEATURE_RTC 1
#define CIOT_CONFIG_FEATURE_EEPROM 0
#define CIOT_CONFIG_FEATURE_FLASH 0
#define CIOT_CONFIG_FEATURE_SDCARD 0
#define CIOT_CONFIG_FEATURE_HARDDISK 0
#define CIOT_CONFIG_FEATURE_DISPLAY 0
#define CIOT_CONFIG_FEATURE_OTA 0
#define CIOT_CONFIG_FEATURE_NTP 0
#define CIOT_CONFIG_FEATURE_MQTT 0
#define CIOT_CONFIG_FEATURE_HTTP_SERVER 0
#define CIOT_CONFIG_FEATURE_HTTP_CLIENT 0
#define CIOT_CONFIG_FEATURE_COAP 0
#define CIOT_CONFIG_FEATURE_MODBUS_RTU 0
#define CIOT_CONFIG_FEATURE_MODBUS_TCP 0
#define CIOT_CONFIG_FEATURE_OPCUA 0
#define CIOT_CONFIG_FEATURE_DATALOGGER 0
#define CIOT_CONFIG_FEATURE_GATEWAY 0
#define CIOT_CONFIG_FEATURE_SCHEDULER 0
#define CIOT_CONFIG_FEATURE_AI 0

#endif  //!__CIOT_DEFAULT_CONFIG__H__