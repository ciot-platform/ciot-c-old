/**
 * @file ciot_app.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_APP__H__
#define __CIOT_APP__H__

#include "ciot_err.h"
#include "ciot_msg_dto.h"
#include "ciot_wifi.h"
#include "ciot_http_server.h"

typedef enum ciot_app_data_type
{
    CIOT_APP_DATA_TYPE_UNKNOWN,
    CIOT_APP_DATA_TYPE_RAW,
    CIOT_APP_DATA_TYPE_JSON_STRING,
} ciot_app_data_type_t;

typedef struct ciot_app_data
{
    ciot_app_data_type_t data_type;
    void *content;
    size_t size;
} ciot_app_data_t;

typedef struct ciot_app_config
{
    ciot_wifi_config_t wifi;
    ciot_http_server_config_t http_server;
} ciot_app_config_t;

typedef struct ciot_app
{
    bool data_received;
    ciot_app_data_t data;
    ciot_msg_response_t result;
} ciot_app_t;

ciot_err_t ciot_app_init(ciot_app_config_t *conf);
ciot_err_t ciot_app_start(ciot_app_config_t *conf);
ciot_err_t ciot_app_send_data(ciot_app_data_t *data);
ciot_err_t ciot_app_data_task();
ciot_err_t ciot_app_send_msg(ciot_msg_t *msg);
ciot_err_t ciot_app_msg_handle(ciot_msg_t *msg);
ciot_err_t ciot_app_wait_process(uint32_t timeout);
ciot_err_t ciot_app_get_msg_response(ciot_msg_response_t *response);

#endif  //!__CIOT_APP__H__