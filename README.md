# CIOT

> **Advise:** This is an work in progress. Don't use it on production environments.

CIOT is an complete IoT standard API implemented in ansi C, compatible with esp-idf, windows, and linux. The CIOT API can be reached by an HTTP Client, but it's designed do easily support any compatible protocol in future. 

Table of contents:

 - [API](#api)
    - [Interfaces](#interfaces)
    - [Message Types](#message-types)
        - [Request](#request)
        - [Config](#config)
        - [Response](#response)

# API

The CIOT API have an unic endpoint. It was done this way to allow porting the API to simpler protocols like uart or modbus. The follow sample code shows how to start an http server on a host machine, and serves APIin "/v1/ciot" endpoint at 80 port.

```c
#include "ciot_app.h"

void app_main(void)
{
    ciot_app_config_t app = {
        .http_server = {
            .port = 80,
            .endpoint = "/v1/ciot"
        }
    };
    ciot_app_start(&app);
}
```

## WiFi

### Get Configuration

#### Request

`POST /v1/ciot`

```json
{
	"type": 1,          /// Message Type   1:request
	"request": 1,       /// Request Type   1:getConfiguration
	"interface": 1      /// Interface Type 1:wifi
}
```

#### Response

`HTTP 200 OK`

```json
{
    "type": 2,                          	/// Message Type   2:Response
	"err_code": 0,                      	/// Error Code     0:noError
	"request": 1,                       	/// Request Type   1:getConfiguration
	"interface": 1,                     	/// Interface Type 1:wifi
	"mode": 0,                          	/// Wifi Mode      0:station, 1:accessPoint
	"ssid": "CABO CANAVERAL",           	/// Wifi SSID
	"password": "16192020",             	/// Wifi password
	"ip": {                             	/// IP Configuration (Optional)
		"dhcp": 3,                      /// DHCP Mode => 0:default, 1:client, 2:server, 3:disabled
		"address": [192,168,1,16],      /// Device Address
		"gateway": [192,168,1,254],     /// Network Gateway
		"mask": [255,255,255,0],        /// Network Mask
		"dns": [192,168,1,254]          /// Netwokr DNS
	},
	"timeout": 10000                    	/// Connection timeout
}
```

### Get Information

#### Request

`POST /v1/ciot`

```json
{
	"type": 1,          /// Message Type   1:request
	"request": 2,       /// Request Type   2:getInformation
	"interface": 1      /// Interface Type 1:wifi
}
```

#### Response

`HTTP 200 OK`

```json
{
    "type": 2,                          /// Message Type   2:Response
	"err_code": 0,                  /// Error Code     0:noError
	"request": 2,                   /// Request Type   2:getInformation
	"interface": 1,                 /// Interface Type 1:wifi
	"mac": [168,66,227,90,20,224],  /// Device MAC
}
```

### Get Status

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
    "type": 2,                          		/// Message Type   2:response
	"err_code": 0,                      		/// Error Code     0:noError
	"request": 3,                       		/// Request Type   3:getStatus
	"interface": 1,                     		/// Interface Type 1:wifi
	"sta": {					/// Connected Station Status
		"ssid": "CABO CANAVERAL",		/// STA SSID
		"rssi": -51,				/// STA rssi
		"authmode": 3,				/// STA auth => 1:open, 2:wep, 3:wpaPsk, 4:wpa2Psk, 5:wpaWpa2Psk, 6:wpa2Enterprise, 7:wpa3Psk, 8:wpa2Wpa3Psk, 9:wapiPsk, 10:owe
		"bssid": [16,71,56,232,157,249]		/// STA bssid (MAC)
	},
	"tcp": {					/// Connected TCP Status
		"state": 3,				/// TCP State => -1:error, 0:stopped, 1:started, 2:connecting, 3:connected
		"connection": 2,			/// TCP Connections Counter
		"ip": [192,168,1,16],			/// TCP IP Address
		"dhcp": {				/// TCP DHCP Status
			"client": 2,			/// TCP DHCP Client Status => 0:init, 1:started, 2:stopped
			"server": 2			/// TCP DHCP Server Status => 0:init, 1:started, 2:stopped
		}
	},
	"scan": 0					/// Wifi Scanner Status => -1:error, 0:idle, 1:scanning, 2:scanned
}
```

### Set Configuration

#### Request

`POST /v1/ciot`

```json
{
	"type": 2,				/// Message Type  	2:setConfiguration
	"interface": 1,				/// Interface Type 	1:wifi
	"ssid": "CABO CANAVERAL",		/// Wifi SSID
	"password": "16192020",			/// Wifi Password
	"mode": 0,				/// Wifi Mode 		0:station, 1:accessPoint
	"ip": {                             	/// IP Configuration (Optional)
		"dhcp": 3,                  	/// DHCP Mode => 0:default, 1:client, 2:server, 3:disabled
		"address": [192,168,1,16],  	/// Device Address
		"gateway": [192,168,1,254], 	/// Network Gateway
		"mask": [255,255,255,0],    	/// Network Mask
		"dns": [192,168,1,254]      	/// Netwokr DNS
	},
	"timeout": 10000                   	/// Connection timeout
}
```

#### Response

`HTTP 200 OK`

```json
{
	"type": 3,		/// Message Type    3:setConfiguration	
	"err_code": 0,		/// Error Code      0:noError
	"request": 0,		/// Request Type    0:none
	"interface": 1,		/// Interface Type 	1:wifi
}
```

## Roadmap:

### v0.1.0: Handle wifi and ntp interfaces, accepting raw and json http requests.

 - [x] wifi interface (idf)
 - [x] ntp interface (idf)
 - [x] http server (idf)
 - [ ] http server (windows)
 - [ ] http server (linux)

### v0.2.0: Handle mqtt interface, accepting raw and json http requests.

 - [ ] mqtt interface (idf)
 - [ ] mqtt interface (windows)
 - [ ] mqtt interface (linux)

### v0.3.0: Handle uart interface outside ciot API.

 - [ ] uart interface (idf)
 - [ ] uart interface (windows)
 - [ ] uart interface (linux)

### v0.4.0: Handle uart with ciot API.

 - [ ] uart interface (idf)
 - [ ] uart interface (windows)
 - [ ] uart interface (linux)
 
