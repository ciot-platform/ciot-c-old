/**
 * @file ciot_system.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_SYSTEM__H__
#define __CIOT_SYSTEM__H__

#include <inttypes.h>
#include <time.h>

#include "ciot_err.h"
#include "ciot_json.h"

#define CIOT_SYSTEM_FEATURES \
    (CIOT_CONFIG_FEATURE_I2C << CIOT_SYSTEM_FEATURE_I2C) | \
    (CIOT_CONFIG_FEATURE_SPI << CIOT_SYSTEM_FEATURE_SPI) | \
    (CIOT_CONFIG_FEATURE_UART << CIOT_SYSTEM_FEATURE_UART) | \
    (CIOT_CONFIG_FEATURE_USB << CIOT_SYSTEM_FEATURE_USB) | \
    (CIOT_CONFIG_FEATURE_ETHERNET << CIOT_SYSTEM_FEATURE_ETHERNET) | \
    (CIOT_CONFIG_FEATURE_WIFI << CIOT_SYSTEM_FEATURE_WIFI) | \
    (CIOT_CONFIG_FEATURE_BLE << CIOT_SYSTEM_FEATURE_BLE) | \
    (CIOT_CONFIG_FEATURE_ZIGBEE << CIOT_SYSTEM_FEATURE_ZIGBEE) | \
    (CIOT_CONFIG_FEATURE_LORA << CIOT_SYSTEM_FEATURE_LORA) | \
    (CIOT_CONFIG_FEATURE_RS232 << CIOT_SYSTEM_FEATURE_RS232) | \
    (CIOT_CONFIG_FEATURE_RS485 << CIOT_SYSTEM_FEATURE_RS485) | \
    (CIOT_CONFIG_FEATURE_CAN << CIOT_SYSTEM_FEATURE_CAN) | \
    (CIOT_CONFIG_FEATURE_GPIO << CIOT_SYSTEM_FEATURE_GPIO) | \
    (CIOT_CONFIG_FEATURE_RTC << CIOT_SYSTEM_FEATURE_RTC) | \
    (CIOT_CONFIG_FEATURE_EEPROM << CIOT_SYSTEM_FEATURE_EEPROM) | \
    (CIOT_CONFIG_FEATURE_FLASH << CIOT_SYSTEM_FEATURE_FLASH) | \
    (CIOT_CONFIG_FEATURE_SDCARD << CIOT_SYSTEM_FEATURE_SDCARD) | \
    (CIOT_CONFIG_FEATURE_HARDDISK << CIOT_SYSTEM_FEATURE_HARDDISK) | \
    (CIOT_CONFIG_FEATURE_DISPLAY << CIOT_SYSTEM_FEATURE_DISPLAY) | \
    (CIOT_CONFIG_FEATURE_STORAGE << CIOT_SYSTEM_FEATURE_STORAGE) | \
    (CIOT_CONFIG_FEATURE_OTA << CIOT_SYSTEM_FEATURE_OTA) | \
    (CIOT_CONFIG_FEATURE_NTP << CIOT_SYSTEM_FEATURE_NTP) | \
    (CIOT_CONFIG_FEATURE_MQTT << CIOT_SYSTEM_FEATURE_MQTT) | \
    (CIOT_CONFIG_FEATURE_HTTP_CLIENT << CIOT_SYSTEM_FEATURE_HTTP_CLIENT) | \
    (CIOT_CONFIG_FEATURE_HTTP_SERVER << CIOT_SYSTEM_FEATURE_HTTP_SERVER) | \
    (CIOT_CONFIG_FEATURE_COAP << CIOT_SYSTEM_FEATURE_COAP) | \
    (CIOT_CONFIG_FEATURE_MODBUS_RTU << CIOT_SYSTEM_FEATURE_MODBUS_RTU) | \
    (CIOT_CONFIG_FEATURE_MODBUS_TCP << CIOT_SYSTEM_FEATURE_MODBUS_TCP) | \
    (CIOT_CONFIG_FEATURE_OPCUA << CIOT_SYSTEM_FEATURE_OPCUA) | \
    (CIOT_CONFIG_FEATURE_DATALOGGER << CIOT_SYSTEM_FEATURE_DATALOGGER) | \
    (CIOT_CONFIG_FEATURE_GATEWAY << CIOT_SYSTEM_FEATURE_GATEWAY) | \
    (CIOT_CONFIG_FEATURE_SCHEDULER << CIOT_SYSTEM_FEATURE_SCHEDULER)

typedef enum ciot_system_device_type
{
    CIOT_SYSTEM_DEVICE_TYPE_UNKNOWN,
    CIOT_SYSTEM_DEVICE_TYPE_GENERIC,
    CIOT_SYSTEM_DEVICE_TYPE_SENSOR,
    CIOT_SYSTEM_DEVICE_TYPE_GATEWAY,
    CIOT_SYSTEM_DEVICE_TYPE_DATALOGGER,
    CIOT_SYSTEM_DEVICE_TYPE_HMI,
} ciot_system_device_type_t;

typedef enum ciot_system_os
{
    CIOT_SYSTEM_OS_UNKNOWN,
    CIOT_SYSTEM_OS_BAREMETAL,
    CIOT_SYSTEM_OS_FREERTOS,
    CIOT_SYSTEM_OS_LINUX,
    CIOT_SYSTEM_OS_WINDOWS,
    CIOT_SYSTEM_OS_MAC,
} ciot_system_os_t;

typedef enum ciot_system_feature
{
    CIOT_SYSTEM_FEATURE_I2C,
    CIOT_SYSTEM_FEATURE_SPI,
    CIOT_SYSTEM_FEATURE_UART,
    CIOT_SYSTEM_FEATURE_USB,
    CIOT_SYSTEM_FEATURE_ETHERNET,
    CIOT_SYSTEM_FEATURE_WIFI,
    CIOT_SYSTEM_FEATURE_BLE,
    CIOT_SYSTEM_FEATURE_ZIGBEE,
    CIOT_SYSTEM_FEATURE_LORA,
    CIOT_SYSTEM_FEATURE_RS232,
    CIOT_SYSTEM_FEATURE_RS485,
    CIOT_SYSTEM_FEATURE_CAN,
    CIOT_SYSTEM_FEATURE_GPIO,
    CIOT_SYSTEM_FEATURE_RTC,
    CIOT_SYSTEM_FEATURE_EEPROM,
    CIOT_SYSTEM_FEATURE_FLASH,
    CIOT_SYSTEM_FEATURE_SDCARD,
    CIOT_SYSTEM_FEATURE_HARDDISK,
    CIOT_SYSTEM_FEATURE_DISPLAY,
    CIOT_SYSTEM_FEATURE_STORAGE,
    CIOT_SYSTEM_FEATURE_OTA,
    CIOT_SYSTEM_FEATURE_NTP,
    CIOT_SYSTEM_FEATURE_MQTT,
    CIOT_SYSTEM_FEATURE_HTTP_CLIENT,
    CIOT_SYSTEM_FEATURE_HTTP_SERVER,
    CIOT_SYSTEM_FEATURE_COAP,
    CIOT_SYSTEM_FEATURE_MODBUS_RTU,
    CIOT_SYSTEM_FEATURE_MODBUS_TCP,
    CIOT_SYSTEM_FEATURE_OPCUA,
    CIOT_SYSTEM_FEATURE_DATALOGGER,
    CIOT_SYSTEM_FEATURE_GATEWAY,
    CIOT_SYSTEM_FEATURE_SCHEDULER,
} ciot_system_feature_t;

typedef enum ciot_system_request
{
    CIOT_SYSTEM_REQUEST_RESTART = 4,
    CIOT_SYSTEM_REQUEST_SAVE_SETTINGS,
    CIOT_SYSTEM_REQUEST_CLEAR_SETTINGS,
} ciot_system_request_t;

typedef struct ciot_system_config
{

} ciot_system_config_t;

typedef struct ciot_system_info
{
    ciot_system_device_type_t device;
    ciot_system_os_t os;
    uint8_t version[5];
    uint8_t date[3];
    char board[16];
    char mcu[16];
    size_t storage;
    uint32_t features;
} ciot_system_info_t;

typedef struct ciot_system_status
{
    time_t time;
    uint32_t memory;
    uint32_t sys_err;
    uint32_t sys_status;
    uint32_t lifetime;
} ciot_system_status_t;

ciot_err_t ciot_system_set_config(ciot_system_config_t *conf);
ciot_err_t ciot_system_get_config(ciot_system_config_t *config);
ciot_err_t ciot_system_get_status(ciot_system_status_t *status);
ciot_err_t ciot_system_get_info(ciot_system_info_t *info);
ciot_err_t ciot_system_set_err_code(uint32_t code);
ciot_err_t ciot_system_set_status_code(uint32_t code);
uint32_t ciot_system_get_err_code(void);
uint32_t ciot_system_get_status_code();
ciot_err_t ciot_system_process_request(ciot_system_request_t request);
ciot_err_t ciot_system_reset(void);

ciot_err_t ciot_system_config_from_json(CJSON_PARAMETERS(ciot_system_config_t));
ciot_err_t ciot_system_config_to_json(CJSON_PARAMETERS(ciot_system_config_t));
ciot_err_t ciot_system_info_from_json(CJSON_PARAMETERS(ciot_system_info_t));
ciot_err_t ciot_system_info_to_json(CJSON_PARAMETERS(ciot_system_info_t));
ciot_err_t ciot_system_status_from_json(CJSON_PARAMETERS(ciot_system_status_t));
ciot_err_t ciot_system_status_to_json(CJSON_PARAMETERS(ciot_system_status_t));

#endif //!__CIOT_SYSTEM__H__