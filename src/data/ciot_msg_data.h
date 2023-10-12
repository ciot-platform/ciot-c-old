/**
 * @file ciot_msg_data.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_MSG_DATA__H__
#define __CIOT_MSG_DATA__H__

#include "ciot_err.h"
#include "ciot_config.h"
#include "ciot_sys_data.h"
#include "ciot_https_data.h"
#include "ciot_httpc_data.h"
#include "ciot_mqttc_data.h"

#define CIOT_MSG_SIZE (sizeof(ciot_msg_t) - sizeof(ciot_msg_data_u))
#define CIOT_MSG_GET_SIZE(type) (CIOT_MSG_SIZE + sizeof(type))

typedef enum __attribute__((packed))
{
    CIOT_MSG_TYPE_UNKOWN,
    CIOT_MSG_TYPE_START,
    CIOT_MSG_TYPE_STOP,
    CIOT_MSG_TYPE_GET_CONFIG,
    CIOT_MSG_TYPE_GET_STATUS,
    CIOT_MSG_TYPE_REQUEST
} ciot_msg_type_t;

typedef enum __attribute__((packed))
{
    CIOT_IFACE_TYPE_UNKNOWN,
    CIOT_IFACE_TYPE_SYSTEM,
    CIOT_IFACE_TYPE_HTTP_SERVER,
    CIOT_IFACE_TYPE_HTTP_CLIENT,
    CIOT_IFACE_TYPE_MQTT,
} ciot_iface_type_t;

typedef union ciot_msg_data
{
    ciot_sys_data_u system;
    ciot_https_data_u https;
    ciot_httpc_data_u httpc;
    ciot_mqttc_data_u mqtt;
} ciot_msg_data_u;

typedef struct __attribute__((packed))
{
    uint8_t id;
    ciot_msg_type_t type;
    ciot_iface_type_t iface;
    ciot_err_t error;
    ciot_msg_data_u data;
} ciot_msg_t;

#endif  //!__CIOT_MSG_DATA__H__