CIOT 0.1.0-alpha
===

> **Advise:** This is an work in progress. Don't use it on production environments.

CIOT is an complete IoT standard API implemented in ansi C, compatible with esp-idf, windows, and linux. The CIOT API can be reached by an HTTP Client, but it's designed do easily support any compatible protocol in future. Each CIOT component can be used individually as an library. Not used components can be disabled on macros described on Features item.

Table of contents:

 - [Features](#features)
 - [API](#api)
    - [Wifi](#wifi)
      - [Get Configuration](#wifi-get-configuration)
      - [Get Information](#wifi-get-information)
      - [Get Status](#wifi-get-status)
      - [Scan](#scan)
      - [Set Configuration](#wifi-set-configuration)
	- [System](#system)
      - [Get Configuration](#system-get-configuration)
      - [Get Information](#system-get-information)
      - [Get Status](#system-get-status)
      - [Restart](#restart)
      - [Save Settings](#save-settings)
      - [Clear Settings](#clear-settings)
      - [Set Configuration](#system-set-configuration)
	- [NTP](#ntp)
      - [Get Configuration](#ntp-get-configuration)
      - [Get Information](#ntp-get-information)
      - [Get Status](#ntp-get-status)
      - [Set Configuration](#ntp-set-configuration)
 - [Roadmap](#roadmap)
   - [v0.1.0](#v010-handle-wifi-and-ntp-interfaces-accepting-raw-and-json)
   - [v0.2.0](#v020-handle-mqtt-interface-accepting-raw-and-json)
   - [v0.3.0](#v030-handle-ota-interface-accepting-raw-and-json)
   - [v0.4.0](#v04)
   
# Features

The current version of CIOT API support the following features:

 - WiFi (idf)
 - NTP (idf)
 - HTTP Server (idf, windows, linux)

You can create an ciot_custom_config.h, to enabled/disabled each feature and customize ciot application. Alternatively you can use CMakeLists.txt to defining and setting values to each configuration macro. To view all available configurations, see the file: main/{target}/ciot_default_config.h. The following macros can be used to enable/disable features at compile time:

```c
/**
 *  Hardware Features
 */
#define CIOT_CONFIG_FEATURE_I2C 0
#define CIOT_CONFIG_FEATURE_SPI 0
#define CIOT_CONFIG_FEATURE_UART 0
#define CIOT_CONFIG_FEATURE_USB 0
#define CIOT_CONFIG_FEATURE_ETHERNET 0
#define CIOT_CONFIG_FEATURE_WIFI 1
#define CIOT_CONFIG_FEATURE_BLE 0
#define CIOT_CONFIG_FEATURE_ZIGBEE 0
#define CIOT_CONFIG_FEATURE_LORA 0
#define CIOT_CONFIG_FEATURE_RS232 0
#define CIOT_CONFIG_FEATURE_RS485 0
#define CIOT_CONFIG_FEATURE_CAN 0
#define CIOT_CONFIG_FEATURE_GPIO 0
#define CIOT_CONFIG_FEATURE_RTC 1
#define CIOT_CONFIG_FEATURE_EEPROM 0
#define CIOT_CONFIG_FEATURE_FLASH 1
#define CIOT_CONFIG_FEATURE_SDCARD 0
#define CIOT_CONFIG_FEATURE_HARDDISK 0
#define CIOT_CONFIG_FEATURE_DISPLAY 0

/**
 *  Software Features
 */
#define CIOT_CONFIG_FEATURE_OTA 0
#define CIOT_CONFIG_FEATURE_NTP 1
#define CIOT_CONFIG_FEATURE_MQTT 0
#define CIOT_CONFIG_FEATURE_HTTP_CLIENT 0
#define CIOT_CONFIG_FEATURE_HTTP_SERVER 1
#define CIOT_CONFIG_FEATURE_COAP 0
#define CIOT_CONFIG_FEATURE_MODBUS_RTU 0
#define CIOT_CONFIG_FEATURE_MODBUS_TCP 0
#define CIOT_CONFIG_FEATURE_OPCUA 0
#define CIOT_CONFIG_FEATURE_DATALOGGER 0
#define CIOT_CONFIG_FEATURE_GATEWAY 0
#define CIOT_CONFIG_FEATURE_SCHEDULER 0
#define CIOT_CONFIG_FEATURE_AI 0
```

If an client try to use an unsupported feature, the CIOT API will return an error code 2 (CIOT_ERR_FEATURE_NOT_SUPPORTED).

```json
{
	"type": 2,
	"request": 1,
	"interface": 65,
	"err_code": 2,
	"err_msg": "Feature Not Supported"
}
```

# API

The CIOT API have an unic endpoint. It's designed to allow portability with simpler protocols like uart or modbus. Although, the default API with the single endpoint, can be easily overrided, because all CIOT application logic is separated from server. The follow sample code shows how to start an http server, and serves the API in "/v1/ciot" endpoint in port 80:

```c
#include "ciot_app.h"

int main(int argc, char **argv)
{
  ciot_app_config_t app = {
    .http_server = {
      .port = 80,
      .endpoint = "/v1/ciot"
    }
  };
  return ciot_app_start(&app);
}

```

## WiFi

### Get Configuration {#wifi-get-configuration}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,         /// Message Type   1:request
  "interface": 1,    /// Interface Type 1:wifi
  "request": 1       /// Request Type   1:getConfiguration
}
```

#### Response

`HTTP 200 OK`

```json
{
  "type": 2,                        /// Message Type   2:Response
  "err_code": 0,                    /// Error Code     0:noError
  "request": 1,                     /// Request Type   1:getConfiguration
  "interface": 1,                   /// Interface Type 1:wifi
  "mode": 0,                        /// Wifi Mode      0:station, 1:accessPoint
  "ssid": "MY SSID",                /// Wifi SSID
  "password": "mypassword",         /// Wifi password
  "ip": {                           /// IP Configuration (Optional)
    "dhcp": 3,                      /// DHCP Mode => 0:default, 1:client, 2:server, 3:disabled
    "address": [192,168,1,16],      /// Device Address
    "gateway": [192,168,1,254],     /// Network Gateway
    "mask": [255,255,255,0],        /// Network Mask
    "dns": [192,168,1,254]          /// Netwokr DNS
  },
  "timeout": 10000                  /// Connection timeout
}
```

### Get Information {#wifi-get-information}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,         /// Message Type   1:request
  "interface": 1,    /// Interface Type 1:wifi
  "request": 2       /// Request Type   2:getInformation
}
```

#### Response

`HTTP 200 OK`

```json
{
  "type": 2,                      /// Message Type   2:Response
  "err_code": 0,                  /// Error Code     0:noError
  "request": 2,                   /// Request Type   2:getInformation
  "interface": 1,                 /// Interface Type 1:wifi
  "available": true,              /// Interface is Available
  "mac": [15,47,53,44,55,77],     /// Device MAC
}
```

### Get Status {#wifi-get-status}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,          /// Message Type   1:request
  "request": 3,       /// Request Type   3:getStatus
  "interface": 1      /// Interface Type 1:wifi
}
```

#### Response

`HTTP 200 OK`

```json
{
  "type": 2,                          /// Message Type   2:response
  "err_code": 0,                      /// Error Code     0:noError
  "request": 3,                       /// Request Type   3:getStatus
  "interface": 1,                     /// Interface Type 1:wifi
  "sta": {                            /// Connected Station Status
    "ssid": "MY-SSID",                /// STA SSID
    "rssi": -51,                      /// STA rssi
    "authmode": 3,                    /// STA auth => 1:open, 2:wep, 3:wpaPsk, 4:wpa2Psk, 5:wpaWpa2Psk, 6:wpa2Enterprise, 7:wpa3Psk, 8:wpa2Wpa3Psk, 9:wapiPsk, 10:owe
    "bssid": [44,55,56,123,222,123]   /// STA bssid (MAC)
  },
  "tcp": {                            /// Connected TCP Status
    "state": 3,                       /// TCP State => -1:error, 0:stopped, 1:started, 2:connecting, 3:connected
    "connection": 2,                  /// TCP Connections Counter
    "ip": [192,168,1,16],             /// TCP IP Address
    "dhcp": {                         /// TCP DHCP Status
      "client": 2,                    /// TCP DHCP Client Status => 0:init, 1:started, 2:stopped
      "server": 2                     /// TCP DHCP Server Status => 0:init, 1:started, 2:stopped
    }
  },
  "scan": 0                           /// Wifi Scanner Status => -1:error, 0:idle, 1:scanning, 2:scanned
}
```

### Scan

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,        /// Message Type   1:request
  "interface": 1,   /// Interface Type 1:wifi
  "request": 4      /// Request Type   4:scan
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,        /// Message Type   1:request
	"err_code": 0,    /// Error Code     0:noError
	"request": 4,     /// Interface Type 1:wifi
	"interface": 1,   /// Request Type   4:scan
	"state": 0,       /// Wifi Scanner Status => -1:error, 0:idle, 1:scanning, 2:scanned
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

### Set Configuration {#wifi-set-configuration}

#### Request

`POST /v1/ciot`

```json
{
  "type": 3,                      /// Message Type  	3:setConfiguration
  "interface": 1,                 /// Interface Type 	1:wifi
  "ssid": "MY-SSID",              /// Wifi SSID
  "password": "my-password",      /// Wifi Password
  "mode": 0,                      /// Wifi Mode => 0:station, 1:accessPoint
  "ip": {                         /// IP Configuration (Optional)
    "dhcp": 3,                    /// DHCP Mode => 0:default, 1:client, 2:server, 3:disabled
    "address": [192,168,1,16],    /// Device Address
    "gateway": [192,168,1,254],   /// Network Gateway
    "mask": [255,255,255,0],      /// Network Mask
    "dns": [192,168,1,254]        /// Netwokr DNS
  },
  "timeout": 10000                /// Connection timeout
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,      /// Message Type    2:response	
	"err_code": 0,  /// Error Code      0:noError
	"request": 0,   /// Request Type    0:none
	"interface": 1, /// Interface Type 	1:wifi
}
```

## System

### Get Configuration {#system-get-configuration}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,         /// Message Type   1:request
  "interface": 64,   /// Interface Type 64:system
  "request": 1       /// Request Type   1:getConfiguration
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,        /// Message Type    2:response	
	"err_code": 0,    /// Error Code      0:noError
	"request": 1,     /// Request Type    1:getConfiguration
	"interface": 64,  /// Interface Type 	64:system
}
```

### Get Information {#system-get-information}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,         /// Message Type   1:request
  "interface": 64,   /// Interface Type 64:system
  "request": 2       /// Request Type   2:getInformation
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,                /// Message Type   1:request
	"err_code": 0,            /// Error Code     0:noError
	"request": 2,             /// Request Type   2:getInformation
	"interface": 64,          /// Interface Type 64:system
	"device": 1,              /// Device Type => 0:unknown, 1:generic, 2:sensor, 3:gateway, 4:datalogger, 5:hmi          
	"os": 2,                  /// Operating System => 0:unknown, 1:bareMetal, 2:freeRTOS, 3:linux, 4:windows, 5:mac
	"version": [0,0,0,1,0],   /// Software Version: 0.0.0.1-alpha
	"date": [19,6,23],        /// Built Date: 19 Jun 2023
	"board": "ESP32 Devkit",  /// Board Name
	"mcu": "ESP32",           /// MCU Name
	"storage": 24576,         /// Storage Size in Bytes
	"features": 8295          /// Fatures Bits Mask
}
```

### Get Status {#system-get-status}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,          /// Message Type   1:request
  "request": 3,       /// Request Type   3:getStatus
  "interface": 64     /// Interface Type 64:system
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,        /// Message Type      1:request
	"err_code": 0,    /// Error Code        0:noError
	"request": 3,     /// Request Type      3:getStatus
	"interface": 64,  /// Interface Type    64:system
	"time": 254,      /// Internal Clock Timestamp in Seconds
	"memory": 218844, /// Free Heap Memory
	"err": 0,         /// System error code
	"status": 0,      /// System status code
	"lifetime": 254   /// Device Lifetime in Seconds
}
```

### Restart

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,        /// Message Type   1:request
  "interface": 1,   /// Interface Type 64:system
  "request": 4      /// Request Type   4:restart
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,        /// Message Type   1:request
	"err_code": 0,    /// Error Code     0:noError
	"request": 4,     /// Request Type   4:restart
	"interface": 64   /// Interface Type 64:system
}
```

### Save Settings

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,        /// Message Type   1:request
  "interface": 1,   /// Interface Type 64:system
  "request": 5      /// Request Type   5:saveSettings
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,        /// Message Type   2:response
	"err_code": 0,    /// Error Code     0:noError
	"request": 5,     /// Request Type   5:saveSettings
	"interface": 64   /// Interface Type 64:system
}
```

### Clear Settings

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,        /// Message Type   1:request
  "interface": 1,   /// Interface Type 64:system
  "request": 6      /// Request Type   6:clearSettings
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,        /// Message Type   2:response
	"err_code": 0,    /// Error Code     0:noError
	"request": 6,     /// Request Type   6:clearSettings
	"interface": 64   /// Interface Type 64:system
}
```

### Set Configuration {#system-set-configuration}

#### Request

`POST /v1/ciot`

```json
{
  "type": 3,        /// Message Type   3:setConfiguration
  "interface": 1,   /// Interface Type 64:system
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,        /// Message Type   2:response
	"err_code": 0,    /// Error Code     0:noError
	"request": 0,     /// Request Type   0:none
	"interface": 64   /// Interface Type 64:system
}
```

## NTP

### Get Configuration {#ntp-get-configuration}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,         /// Message Type   1:request
  "interface": 65,   /// Interface Type 65:ntp
  "request": 1       /// Request Type   1:getConfiguration
}
```

#### Response

`HTTP 200 OK`

```json
{
  "type": 2,                  /// Message Type   1:response
  "err_code": 0,              /// Error Code     0:noError
  "request": 1,               /// Request Type   1:getConfiguration          
  "interface": 65,            /// Interface Type 65:ntp
  "op_mode": 0,               /// Operation Mode => 0:pool, 1:listenOnly
  "sync_mode": 0,             /// Sync Mode => 0:immediatly, 1:smooth
  "sync_interval": 3600000,   /// Sync Interval in seconds
  "timezone": "<-03>3",       /// Unix Timezone
  "timeout": 10000            /// Connection timeout                
}
```

### Get Information {#ntp-get-information}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,        /// Message Type   1:request
  "interface": 65,  /// Interface Type 65:ntp
  "request": 2      /// Request Type   2:getInformation
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,          /// Message Type   1:request
	"err_code": 0,      /// Error Code     0:noError
	"request": 2,       /// Request Type   2:getInformation
	"interface": 65,    /// Interface Type 65:ntp
  "available": true   /// Interface is Available
}
```

### Get Status {#ntp-get-status}

#### Request

`POST /v1/ciot`

```json
{
  "type": 1,          /// Message Type   1:request
  "request": 3,       /// Request Type   3:getStatus
  "interface": 65     /// Interface Type 65:ntp
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 2,          /// Message Type   1:request
	"err_code": 0,      /// Error Code     0:noError
	"request": 3,       /// Request Type   3:getStatus
	"interface": 65,    /// Interface Type 65:ntp
	"init": true,       /// Initialized
	"sync": false,      /// Syncronized
	"state": 0,         /// NTP Sync State 0:reseted, 1:completed, 2:inProgress
  "sync_count": 0,    /// Sync counter
	"last_sync": 0      /// Last sync timestamp
}
```

### Set Configuration {#ntp-set-configuration}

#### Request

`POST /v1/ciot`

```json
{
  "type": 3,                      /// Message Type   3:setConfiguration
  "interface": 65,                /// Interface Type 65:ntp
  "op_mode": 0,                   /// Operation Mode => 0:pool, 1:listenOnly
  "sync_mode": 0,                 /// Sync Mode => 0:immediatly, 1:smooth
  "sync_interval": 3600000,       /// Sync Interval in seconds
  "timezone": "<-03>3",           /// Unix Timezone (Optional)
  "server1": "pool.ntp.org",      /// Main ntp server
	"server2": "time.google.com",   /// Backup ntp server (Optional)
	"server3": "gps.ntp.br",        /// Backup ntp server (Optional)
  "timeout": 10000                /// Connection timeout                
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

 - [ ] mqtt interface (idf)
 - [ ] mqtt interface (windows)
 - [ ] mqtt interface (linux)

### v0.3.0: Handle ota interface, accepting raw and json

 - [ ] ota interface (idf)
 - [ ] ota interface (windows)
 - [ ] ota interface (linux)

### v0.4.0: Handle uart interface outside ciot API.

 - [ ] uart interface (idf)
 - [ ] uart interface (windows)
 - [ ] uart interface (linux)
