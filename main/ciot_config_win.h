/**
 * @file ciot_custom_config_win.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_CUSTOM_CONFIG_WIN__H__
#define __CIOT_CUSTOM_CONFIG_WIN__H__

#define CIOT_CONFIG_WIFI_AP_FILENAME "wifi_ap.dat"
#define CIOT_CONFIG_WIFI_STA_FILENAME "wifi_sta.dat"
#define CIOT_CONFIG_NTP_FILENAME "ntp.dat"

#define CIOT_CONFIG_VERSION { 0, 0, 0, 1 }
#define CIOT_CONFIG_DATE { 30, 06, 23 }
#define CIOT_CONFIG_DEFAULT_TIMEZONE "<-03>3"

#define CIOT_CONFIG_DEVICE_TYPE CIOT_SYSTEM_DEVICE_TYPE_GENERIC
#define CIOT_CONFIG_OS CIOT_SYSTEM_OS_WINDOWS
#define CIOT_CONFIG_BOARD "WORKSTATION"
#define CIOT_CONFIG_MCU "Ryzen 7 3700X"

#define CIOT_CONFIG_API_ERROR_MESSAGES 1
#define CIOT_CONFIG_CONSOLE_ERROR_MESSAGES 1

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
#define CIOT_CONFIG_FEATURE_HARDDISK 1
#define CIOT_CONFIG_FEATURE_DISPLAY 1
#define CIOT_CONFIG_FEATURE_STORAGE 1
#define CIOT_CONFIG_FEATURE_OTA 0
#define CIOT_CONFIG_FEATURE_NTP 0
#define CIOT_CONFIG_FEATURE_MQTT 0
#define CIOT_CONFIG_FEATURE_HTTP_SERVER 1
#define CIOT_CONFIG_FEATURE_HTTP_CLIENT 0
#define CIOT_CONFIG_FEATURE_COAP 0
#define CIOT_CONFIG_FEATURE_MODBUS_RTU 0
#define CIOT_CONFIG_FEATURE_MODBUS_TCP 0
#define CIOT_CONFIG_FEATURE_OPCUA 0
#define CIOT_CONFIG_FEATURE_DATALOGGER 0
#define CIOT_CONFIG_FEATURE_GATEWAY 0
#define CIOT_CONFIG_FEATURE_SCHEDULER 0

#define CIOT_CONFIG_FEATURES CIOT_SYSTEM_FEATURES                                            

#endif  //!__CIOT_CUSTOM_CONFIG_WIN__H__