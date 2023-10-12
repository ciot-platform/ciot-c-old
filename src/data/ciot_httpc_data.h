/**
 * @file ciot_httpc_data.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_HTTPC_DATA__H__
#define __CIOT_HTTPC_DATA__H__

#include <inttypes.h>
#include "ciot_config.h"

#define CIOT_HTTPC_URL_LEN 64
#define CIOT_HTTPC_METHOD_LEN 8
#define CIOT_HTTPC_BODY_LEN 256

typedef enum __attribute__((packed))
{
    CIOT_HTTPC_STATE_IDLE,
    CIOT_HTTPC_STATE_CONNECTING,
    CIOT_HTTPC_STATE_CONNECTED,
    CIOT_HTTPC_STATE_TIMEOUT,
} ciot_httpc_state_t;

typedef enum __attribute__((packed))
{
    CIOT_HTTPC_REQ_UNKNOWN,
    CIOT_HTTPC_REQ_SEND_DATA,
} ciot_httpc_req_id_t;

typedef struct __attribute__((packed))
{
    char url[CIOT_HTTPC_URL_LEN];
    char method[CIOT_HTTPC_METHOD_LEN];
    uint16_t timeout;
} ciot_httpc_cfg_t;

typedef struct __attribute__((packed))
{
    ciot_httpc_state_t state;
} ciot_httpc_status_t;

typedef struct __attribute__((packed))
{
    ciot_httpc_cfg_t cfg;
    uint8_t body[CIOT_HTTPC_BODY_LEN];
    int content_length;
} ciot_httpc_req_send_t;

typedef union ciot_httpc_req_data
{
    ciot_httpc_req_send_t send;
} ciot_httpc_req_data_u;

typedef struct __attribute__((packed))
{
    ciot_httpc_req_id_t id;
    ciot_httpc_req_data_u data;
} ciot_httpc_req_t;

typedef struct __attribute__((packed))
{
    char *url;
    void *data;
    int size;
} ciot_httpc_msg_t;

typedef union ciot_httpc_data
{
    #if CIOT_CONFIG_FEATURE_HTTPC
    ciot_httpc_cfg_t config;
    ciot_httpc_status_t status;
    ciot_httpc_req_t request;
    ciot_httpc_msg_t msg;
    #endif
} ciot_httpc_data_u;

#endif  //!__CIOT_HTTPC_DATA__H__