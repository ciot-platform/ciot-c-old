/**
 * @file ciot_http_server.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_HTTP_SERVER__H__
#define __CIOT_HTTP_SERVER__H__

#include <inttypes.h>

#include "ciot_msg_dto.h"
#include "ciot_err.h"

#define CIOT_HTTP_CONTENT_TYPE_OCTET "application/octet-stream"
#define CIOT_HTTP_CONTENT_TYPE_JSON "application/json"

typedef struct ciot_http_server_config
{
    uint32_t port;
    char endpoint[48];
} ciot_http_server_config_t;

ciot_err_t ciot_http_server_start(ciot_http_server_config_t *conf);
ciot_err_t ciot_http_server_stop(void);
ciot_err_t ciot_http_server_set_response(ciot_msg_t *msg);

#endif  //!__CIOT_HTTP_SERVER__H__