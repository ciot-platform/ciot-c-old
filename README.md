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

The CIOT API can receive "messages" to interact with your own "interfaces". We have some message types, and each message type have it own particularities. All CIOT device features are called "interfaces". So, to send an configuration message to wifi interface for example, the message needs to declare type as CIOT_MSG_TYPE_CONFIG, and specify target interface as CIOT_MSG_IF_WIFI. These definitions are enumerations and can be represented by a unsigned integer number.

## Interfaces

An interface its an representation of a device feature. That can be a hardware or software feature. All hardware interfaces will be enumerated from id=1, to id=63. And all software interfaces will be enumarated from id=64 to id=255. So, with only one byte we can reference any device feature, and we can knwow if it's an hardware feature or a software feature. 

```c
typedef enum ciot_msg_interface
{
    CIOT_MSG_IF_UNKNOWN = 0,
    CIOT_MSG_IF_WIFI = 1,
    CIOT_MSG_IF_ETHERNET = 2,
    CIOT_MSG_IF_UART = 2,
    CIOT_MSG_IF_
    CIOT_MSG_IF_SYSTEM = 64,
    CIOT_MSG_IF_NTP = 65,
} ciot_msg_interface_t;

```

## Message Types

The CIOT API support three types of messages:

### Request:

An request message can be used to get an specific resource from an interface. All interfaces must implements methods to get these resources. And we have three types of resources in each interface:

 - Get Config: get interface active configuration
 - Get Information: get interface information. Information is all static data about interface.
 - Get Status: get interface status. Status is all dynamic data about interface.
 - Action: send an action request to an interface.

```c
typedef enum ciot_msg_request_type
{
    CIOT_MSG_REQUEST_NONE,
    CIOT_MSG_REQUEST_GET_CONFIG,
    CIOT_MSG_REQUEST_GET_INFO,
    CIOT_MSG_REQUEST_GET_STATUS,
} ciot_msg_request_type_t;

```

### Config:

An config message, it's an message to set the selected interface configuration. These message type can be used to configure the wifi station interface for example, sending an ssid and password to connect device to an known wifi access point. Each interface, have an specific configuration struct. So, to use an interface, it's recommended to read your own documentation.

### Response:

CIOT API will send an response after receives an message from other device. Each message type received by API will generate an different response data. If client sends an request, reponse can contains de requested resource content. If client sends an configuration, response will indicate sucess or error. 

Message example:

```json
{
    "type": 2,                  // Message Type: Config
    "interface":                // Message Interface: WiFi
    "ssid": "MY-SSID",          // WiFi SSID
    "password": "MY-PASSWORD",  // WiFi Password
    "mode": 0,                  // WiFi Mode: Station
    "timeout": 10000            // Connection Timeout
}
```

An more completed API documentation will be elaborated after some project progress.

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
 