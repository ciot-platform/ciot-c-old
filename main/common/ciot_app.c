/**
 * @file ciot_app.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_app.h"
#include "ciot_tasks.h"
#include "ciot_storage.h"

static ciot_app_t this;

static ciot_err_t ciot_app_request_handle(ciot_msg_request_t *request);
static ciot_err_t ciot_app_config_handle(ciot_msg_config_t  *config);
static ciot_err_t ciot_app_get_config_handle(ciot_msg_interface_t interface);
static ciot_err_t ciot_app_get_info_handle(ciot_msg_interface_t interface);
static ciot_err_t ciot_app_get_status_handle(ciot_msg_interface_t interface);
static ciot_err_t ciot_app_other_request_handle(ciot_msg_request_t *request);

ciot_err_t ciot_app_init(ciot_app_config_t *conf)
{
    ciot_err_t err;

    err = ciot_storage_init();
    if (err != CIOT_ERR_OK)
    {
        return err;
    }

    err = ciot_wifi_set_config(CIOT_WIFI_TYPE_AP, &conf->ap);
    if (err != CIOT_ERR_OK)
    {
        return err;
    }

    err = ciot_http_server_start(&conf->http_server);
    if (err != CIOT_ERR_OK)
    {
        return err;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_app_msg_handle(ciot_msg_t *msg)
{
    ciot_err_t err;

    switch (msg->type)
    {
    case CIOT_MSG_TYPE_REQUEST:
        err = ciot_app_request_handle(&msg->data.request);
        break;
    case CIOT_MSG_TYPE_CONFIG:
        err = ciot_app_config_handle(&msg->data.config);
        break;
    default:
        err = CIOT_ERR_INVALID_TYPE;
        break;
    }

    this.result.err_code = err;

    return err;
}

ciot_err_t ciot_app_get_msg_response(ciot_msg_response_t *response)
{
    *response = this.result;
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_app_request_handle(ciot_msg_request_t *request)
{
    this.result.request = request->request;
    switch (request->request)
    {
    case CIOT_MSG_REQUEST_GET_CONFIG:
        return ciot_app_get_config_handle(request->interface);
    case CIOT_MSG_REQUEST_GET_INFO:
        return ciot_app_get_info_handle(request->interface);
    case CIOT_MSG_REQUEST_GET_STATUS:
        return ciot_app_get_status_handle(request->interface);
    default:
        return ciot_app_other_request_handle(request);
    }
}

static ciot_err_t ciot_app_config_handle(ciot_msg_config_t *config)
{
    switch (config->interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_set_config(CIOT_WIFI_TYPE_STA, &config->data.wifi);
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_set_config(&config->data.system);
    case CIOT_MSG_IF_NTP:
        return ciot_ntp_set_config(&config->data.ntp);
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_get_config_handle(ciot_msg_interface_t interface)
{
    this.result.data.config.interface = interface;
    switch (interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_get_config(CIOT_WIFI_TYPE_STA, &this.result.data.config.data.wifi);
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_get_config(&this.result.data.config.data.system);
    case CIOT_MSG_IF_NTP:
        return ciot_ntp_get_config(&this.result.data.config.data.ntp);
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_get_info_handle(ciot_msg_interface_t interface)
{
    this.result.data.info.interface = interface;
    switch (interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_get_info(CIOT_WIFI_TYPE_STA, &this.result.data.info.data.wifi);
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_get_info(&this.result.data.info.data.system);
    case CIOT_MSG_IF_NTP:
        return ciot_ntp_get_info(&this.result.data.info.data.ntp);
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_get_status_handle(ciot_msg_interface_t interface)
{
    this.result.data.status.interface = interface;
    switch (interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_get_status(CIOT_WIFI_TYPE_STA, &this.result.data.status.data.wifi);
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_get_status(&this.result.data.status.data.system);
    case CIOT_MSG_IF_NTP:
        return ciot_ntp_get_status(&this.result.data.status.data.ntp);
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_other_request_handle(ciot_msg_request_t *request)
{
    switch (request->interface)
    {
    case CIOT_MSG_IF_WIFI:
        return CIOT_ERR_INVALID_REQUEST;        
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_process_request(request->request);
    case CIOT_MSG_IF_NTP:
        return CIOT_ERR_INVALID_REQUEST;
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}