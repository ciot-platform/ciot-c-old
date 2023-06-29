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

#include <stdlib.h>

#include "ciot_config.h"
#include "ciot_app.h"
#include "ciot_tasks.h"
#include "ciot_storage.h"
#include "ciot_settings.h"

static ciot_app_t this;

static ciot_err_t ciot_app_request_handle(ciot_msg_request_t *request);
static ciot_err_t ciot_app_config_handle(ciot_msg_config_t  *config);
static ciot_err_t ciot_app_get_config_handle(ciot_msg_interface_t interface);
static ciot_err_t ciot_app_get_info_handle(ciot_msg_interface_t interface);
static ciot_err_t ciot_app_get_status_handle(ciot_msg_interface_t interface);
static ciot_err_t ciot_app_other_request_handle(ciot_msg_request_t *request);
static ciot_err_t ciot_app_init_interface(char *config_filename, size_t config_size, ciot_err_t (*config_func)(void *));

ciot_err_t ciot_app_init(ciot_app_config_t *conf)
{
    ciot_err_t err = CIOT_ERR_OK;
    ciot_wifi_config_t wifi_ap;

    err += ciot_storage_init();

    err = ciot_storage_load_data(&wifi_ap, sizeof(wifi_ap), CIOT_CONFIG_WIFI_AP_FILENAME);
    if (err == CIOT_ERR_OK)
    {
        err += ciot_wifi_set_config(&wifi_ap);
    }
    else
    {
        err += ciot_wifi_set_config(&conf->wifi);
    }

    err += ciot_http_server_start(&conf->http_server);
    err += ciot_app_init_interface(CIOT_CONFIG_WIFI_STA_FILENAME, sizeof(ciot_wifi_config_t), (ciot_err_t (*)(void *))ciot_wifi_set_config);
    err += ciot_app_init_interface(CIOT_CONFIG_NTP_FILENAME, sizeof(ciot_ntp_config_t), (ciot_err_t (*)(void *))ciot_ntp_set_config);

    return err != CIOT_ERR_OK ? CIOT_ERR_FAIL : CIOT_ERR_OK;
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
    this.result.request = *request;
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
        return ciot_wifi_set_config(&config->data.wifi);
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
    switch (interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_get_config(CIOT_WIFI_IF_STA, &this.result.data.config.wifi);
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_get_config(&this.result.data.config.system);
    case CIOT_MSG_IF_NTP:
        return ciot_ntp_get_config(&this.result.data.config.ntp);
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_get_info_handle(ciot_msg_interface_t interface)
{
    switch (interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_get_info(CIOT_WIFI_IF_STA, &this.result.data.info.wifi);
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_get_info(&this.result.data.info.system);
    case CIOT_MSG_IF_NTP:
        return ciot_ntp_get_info(&this.result.data.info.ntp);
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_get_status_handle(ciot_msg_interface_t interface)
{
    switch (interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_get_status(CIOT_WIFI_IF_STA, &this.result.data.status.wifi);
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_get_status(&this.result.data.status.system);
    case CIOT_MSG_IF_NTP:
        return ciot_ntp_get_status(&this.result.data.status.ntp);
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_other_request_handle(ciot_msg_request_t *request)
{
    switch (request->interface)
    {
    case CIOT_MSG_IF_WIFI:
        return ciot_wifi_process_request(request->request, &this.result.data.wifi);    
    case CIOT_MSG_IF_SYSTEM:
        return ciot_system_process_request(request->request);
    case CIOT_MSG_IF_NTP:
        return CIOT_ERR_INVALID_REQUEST;
    default:
        return CIOT_ERR_INVALID_INTERFACE;
    }
}

static ciot_err_t ciot_app_init_interface(char *config_filename, size_t config_size, ciot_err_t (*config_func)(void *))
{
    void *data = malloc(config_size);
    if(data == NULL)
    {
        return CIOT_ERR_MEMORY_ALLOCATION;
    }
    if(config_func == NULL)
    {
        return CIOT_ERR_NULL_REFERENCE;
    }
    if(ciot_storage_load_data(data, config_size, config_filename) == CIOT_ERR_OK)
    {
        return config_func(data);
    }
    return CIOT_ERR_OK;
}
