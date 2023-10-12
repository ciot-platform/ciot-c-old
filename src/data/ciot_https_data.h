/**
 * @file ciot_https_data.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_HTTPS_DATA__H__
#define __CIOT_HTTPS_DATA__H__

#include <inttypes.h>
#include "ciot_config.h"

#define CIOT_HTTPS_ADDRESS_LEN 64
#define CIOT_HTTPS_ROUTE_LEN 32
#define CIOT_HTTPS_METHOD_LEN 8

typedef enum __attribute__((packed))
{
    CIOT_HTTPS_STATE_STOPPED,
    CIOT_HTTPS_STATE_STARTED,
    CIOT_HTTPS_STATE_ERROR,
} ciot_https_state_t;

typedef enum __attribute__((packed))
{
    CIOT_HTTPS_REQ_UNKNOWN,
} ciot_https_req_id_t;

typedef struct __attribute__((packed))
{
    char address[CIOT_HTTPS_ADDRESS_LEN];
    char route[CIOT_HTTPS_ROUTE_LEN];
} ciot_https_cfg_t;

typedef struct __attribute__((packed))
{
    ciot_https_state_t state;
} ciot_https_status_t;

typedef union ciot_https_req_data
{

} ciot_https_req_data_u;

typedef struct __attribute__((packed))
{

} ciot_https_req_t;

typedef struct __attribute__((packed))
{
    char *url;
    char *method;
    uint8_t *data;
    int size;
} ciot_https_event_msg_t;

typedef union ciot_https_data
{
    #if CIOT_CONFIG_FEATURE_HTTPS
    ciot_https_cfg_t config;
    ciot_https_status_t status;
    ciot_https_req_t request;
    ciot_https_event_msg_t msg;
    #endif
} ciot_https_data_u;

#endif  //!__CIOT_HTTPS_DATA__H__