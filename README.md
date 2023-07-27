CIOT 0.2.0-alpha
===

> **Advise:** This is an work in progress. Don't use it on production environments.

CIOT is an complete IoT standard API implemented in ansi C, compatible with esp-idf, windows, and linux. The CIOT API can be reached by an HTTP Client, but it's designed to easily support any compatible protocol in future. Each CIOT component can be used individually as an library. Not used components can be disabled on macros described on Features item.

Table of contents:

 - [Features](#features)
 - [API](#api)
    - [Wifi](#wifi)
      - [Get Configuration](#get-configuration)
      - [Get Information](#get-information)
      - [Get Status](#get-status)
      - [Scan](#scan)
      - [Set Configuration](#set-configuration)
    - [System](#system)
        - [Get Configuration](#get-configuration-1)
        - [Get Information](#get-information-1)
        - [Get Status](#get-status-1)
        - [Restart](#restart)
        - [Save Settings](#save-settings)
        - [Clear Settings](#clear-settings)
        - [Set Configuration](#set-configuration-1)
    - [NTP](#ntp)
        - [Get Configuration](#get-configuration-2)
        - [Get Information](#get-information-2)
        - [Get Status](#get-status-2)
        - [Set Configuration](#set-configuration-2)
    - [MQTT](#mqtt)
        - [Get Configuration](#get-configuration-3)
        - [Get Information](#get-information-3)
        - [Get Status](#get-status-3)
        - [Set Configuration](#set-configuration-3)
    - [OTA](#ota)
        - [Get Configuration](#get-configuration-4)
        - [Get Information](#get-information-4)
        - [Get Status](#get-status-4)
        - [Set Configuration](#set-configuration-4)
 - [Roadmap](#roadmap)
   - [v0.1.0](#v010-handle-wifi-and-ntp-interfaces-accepting-raw-and-json)
   - [v0.2.0](#v020-handle-mqtt-interface-accepting-raw-and-json)
   - [v0.3.0](#v030-handle-ota-interface-accepting-raw-and-json)
   - [v0.4.0](#v040-handle-uart-interface-outside-ciot-api)

# Features

The current version of CIOT API support the following features:

 - WiFi (idf)
 - NTP (idf)
 - HTTP Server (idf, windows, linux)
 - MQTT Server (idf, windows, linux)
 - OTA (idf)

You can create an ciot_custom_config.h, to enabled/disabled each feature and customize ciot application. Alternatively you can use CMakeLists.txt to defining and setting values to each configuration macro. To view all available configurations, see the file: main/{target}/ciot_default_config.h. The following macros can be used to enable/disable features at compile time:

```c
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
#define CIOT_CONFIG_FEATURE_SYS 1
#define CIOT_CONFIG_FEATURE_OTA 0
#define CIOT_CONFIG_FEATURE_NTP 0
#define CIOT_CONFIG_FEATURE_MQTT 1
#define CIOT_CONFIG_FEATURE_HTTP_SERVER 1
#define CIOT_CONFIG_FEATURE_HTTP_CLIENT 0
#define CIOT_CONFIG_FEATURE_COAP 0
#define CIOT_CONFIG_FEATURE_MODBUS_RTU 0
#define CIOT_CONFIG_FEATURE_MODBUS_TCP 0
#define CIOT_CONFIG_FEATURE_OPCUA 0
#define CIOT_CONFIG_FEATURE_DATALOGGER 0
#define CIOT_CONFIG_FEATURE_GATEWAY 0
```

If an client try to use an disabled feature, the CIOT API will return an error code 10(CIOT_ERR_INVALID_INTERFACE).

```json
{
	"msg": 2, 
	"request": 1,
	"interface": 65,
	"err_code": 10,
	"err_msg": "Invalid Interface"
}
```

# API

The CIOT API have an unic endpoint. It's designed to allow portability with simpler protocols like uart or modbus. Although, the default API with the single endpoint, can be easily overrided, because all CIOT application logic is separated from server. The follow sample code shows how to start an http server, and serves the API in "/v1/ciot" endpoint in port 8000:

```c
#include "ciot_app.h"

int main(int argc, char **argv)
{
  ciot_app_config_t app = {
    .http_server = {
      .port = 8000,
      .endpoint = "/v1/ciot"
    }
  };
  return ciot_app_start(&app);
}

```

## WiFi

### Get Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 1,
  "request": 1
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 1,
  "interface": 1,
  "mode": 0,
  "ssid": "MY SSID",
  "password": "mypassword",
  "ip": {
    "dhcp": 3,
    "address": [192,168,1,16],
    "gateway": [192,168,1,254],
    "mask": [255,255,255,0],
    "dns": [192,168,1,254]
  },
  "timeout": 10000
}
```

### Get Information

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 1,
  "request": 2
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 2,
  "interface": 1,
  "available": true,
  "mac": [15,47,53,44,55,77],
}
```

### Get Status

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "request": 3,
  "interface": 1
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 3,
  "interface": 1,
  "sta": {
    "ssid": "MY-SSID",
    "rssi": -51,
    "authmode": 3,
    "bssid": [44,55,56,123,222,123]
  },
  "tcp": {
    "state": 3,
    "connection": 2,
    "ip": [192,168,1,16],
    "dhcp": {
      "client": 2,
      "server": 2
    }
  },
  "scan": 0
}
```

### Scan

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 1,
  "request": 4
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 4,
  "interface": 1,
  "state": 0,
  "result": [
    {
      "ssid": "CABO CANAVERAL 3",
      "rssi": -6,
      "authmode": 4,
      "bssid": [40,59,130,108,154,68]
    },
    {
      "ssid": "CABO CANAVERAL 2",
      "rssi": -23,
      "authmode": 4,
      "bssid": [128,143,232,41,30,97]
    },
    {
      "ssid": "CABO CANAVERAL",
      "rssi": -43,
      "authmode": 3,
      "bssid": [16,71,56,232,157,249]
    },
    {
      "ssid": "OMELHOR",
      "rssi": -69,
      "authmode": 3,
      "bssid": [144,117,188,70,37,9]
    },
    {
      "ssid": "Solar-WiFi196W0039",
      "rssi": -74,
      "authmode": 3,
      "bssid": [152,216,99,95,150,255]
    },
    {
      "ssid": "Rosana Salgados 2.4G",
      "rssi": -80,
      "authmode": 3,
      "bssid": [80,27,50,183,145,249]
    },
    {
      "ssid": "Falcao 2g",
      "rssi": -83,
      "authmode": 4,
      "bssid": [48,223,141,5,3,217]
    },
    {
      "ssid": "Geraldo OI FIBRA 2g",
      "rssi": -87,
      "authmode": 3,
      "bssid": [240,104,101,67,127,121]
    },
    {
      "ssid": "Home network",
      "rssi": -87,
      "authmode": 3,
      "bssid": [28,59,243,55,78,130]
    },
    {
      "ssid": "JoaoMiguel",
      "rssi": -90,
      "authmode": 3,
      "bssid": [160,28,141,45,249,128]
    }
  ]
}
```

### Set Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 3,
  "interface": 1,
  "ssid": "MY-SSID",
  "password": "my-password",
  "mode": 0,
  "ip": {
    "dhcp": 3,
    "address": [192,168,1,16],
    "gateway": [192,168,1,254],
    "mask": [255,255,255,0],
    "dns": [192,168,1,254]
  },
  "timeout": 10000
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "interface": 1
}
```

## System

### Get Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 64,
  "request": 1
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 1,
  "interface": 64,
}
```

### Get Information

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 64,
  "request": 2
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 2,
  "interface": 64,
  "device": 1,
  "os": 2,
  "version": [0,0,0,1,0],
  "date": [19,6,23],
  "board": "ESP32 Devkit",
  "mcu": "ESP32",
  "storage": 24576,
  "features": 8295
}
```

### Get Status

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "request": 3,
  "interface": 64
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 3,
  "interface": 64,
  "time": 254,
  "memory": 218844
  "err": 0,
  "status": 0,
  "lifetime": 254
}
```

### Restart

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 1,
  "request": 4
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 4,
  "interface": 64
}
```

### Save Settings

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 1,
  "request": 5
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 5,
  "interface": 64
}
```

### Clear Settings

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 1,
  "request": 6
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 6,
  "interface": 64
}
```

### Set Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 3,
  "interface": 1,
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "interface": 64
}
```

## NTP

### Get Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 65,
  "request": 1
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 1,
  "interface": 65,
  "op_mode": 0,
  "sync_mode": 0,
  "sync_interval": 3600000,
  "timezone": "<-03>3",
  "timeout": 10000
}
```

### Get Information

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 65,
  "request": 2
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 2,
  "interface": 65,
  "available": true
}
```

### Get Status

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "request": 3,
  "interface": 65
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 3,
  "interface": 65,
  "init": true,
  "sync": false,
  "state": 0,
  "sync_count": 0,
  "last_sync": 0
}
```

### Set Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 3,
  "interface": 65,
  "op_mode": 0,
  "sync_mode": 0,
  "sync_interval": 3600000,
  "timezone": "<-03>3",
  "server1": "pool.ntp.org",
  "server2": "time.google.com",
  "server3": "gps.ntp.br",
  "timeout": 10000
}
```

## MQTT

### Get Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 66,
  "request": 1
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 1,
  "interface": 66,
  "config": 3,
  "connection": {
    "url": "mqtt://test.mosquitto.org",
    "port": 1883,
    "qos": 1
  },
  "topics": {
    "message": "CIOT/message",
    "response": "CIOT/response",
    "data_type": 2
  }
}
```

### Get Information

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 66,
  "request": 2
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 2,
  "interface": 66,
  "version": 0
}
```

### Get Status

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "request": 3,
  "interface": 66
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 3,
  "interface": 66,
  "state": 2,
  "connection": 1
}
```

## OTA

### Get Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 67,
  "request": 1
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 1,
  "interface": 67,
  "url": "https://github.com/ciot-platform/ciot-c/raw/feat/ota/bin/esp32/ciot.bin",
  "force": false,
  "encrypted": false,
  "timeout": 0,
  "restart": false
}
```

### Get Information

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "interface": 67,
  "request": 2
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 2,
  "interface": 67
}
```

### Get Status

#### Request

`POST /v1/ciot`

```json
{
  "msg": 1,
  "request": 3,
  "interface": 67
}
```

#### Response

`HTTP 200 OK`

```json
{
  "msg": 2,
  "request": 3,
  "interface": 67,
  "state": 9,
  "image_size": 1032336,
  "image_read": 1033360
}
```

### Set Configuration

#### Request

`POST /v1/ciot`

```json
{
  "msg": 2,
  "interface": 67,
  "url": "https://github.com/ciot-platform/ciot-c/raw/feat/ota/bin/esp32/ciot.bin",
  "force": false,
  "encrypted": false,
  "timeout": 0,
  "restart": false
}
```

## Roadmap:

### v0.1.0: Handle wifi and ntp interfaces, accepting raw and json

 - [x] wifi interface (idf)
 - [x] ntp interface (idf)
 - [x] http server (idf)
 - [x] http server (windows)
 - [x] http server (linux)

### v0.2.0: Handle mqtt interface, accepting raw and json

 - [x] mqtt interface (idf)
 - [x] mqtt interface (windows)
 - [x] mqtt interface (linux)

### v0.3.0: Handle ota interface, accepting raw and json

 - [x] ota interface (idf)
 - [ ] ota interface (windows)
 - [ ] ota interface (linux)

### v0.4.0: Handle uart interface outside ciot API.

 - [ ] uart interface (idf)
 - [ ] uart interface (windows)
 - [ ] uart interface (linux)
