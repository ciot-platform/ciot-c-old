/**
 * @file ciot_tcp.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_TCP__H__
#define __CIOT_TCP__H__

#include <inttypes.h>
#include <stdbool.h>

#include "ciot_json.h"

typedef enum ciot_tcp_dhcp_type {
    CIOT_TCP_DHCP_TYPE_DISABLED,
    CIOT_TCP_DHCP_TYPE_SERVER,
    CIOT_TCP_DHCP_TYPE_CLIENT,
} ciot_tcp_dhcp_type_t;

typedef enum ciot_tcp_dhcp_status {
    CIOT_TCP_DHCP_STATUS_INIT,
    CIOT_TCP_DHCP_STATUS_STARTED,
    CIOT_TCP_DHCP_STATUS_STOPPED,
} ciot_tcp_dhcp_status_t;

typedef enum ciot_tcp_dhcp_config {
    CIOT_TCP_DHCP_CONFIG_NO_CHANGE,
    CIOT_TCP_DHCP_CONFIG_CLIENT,
    CIOT_TCP_DHCP_CONFIG_SERVER,
    CIOT_TCP_DHCP_CONFIG_DISABLED,
} ciot_tcp_dhcp_config_t;

typedef struct ciot_tcp_ip_config
{
    ciot_tcp_dhcp_config_t dhcp;
    uint8_t address[4];
    uint8_t gateway[4];
    uint8_t mask[4];
    uint8_t dns[4];
} ciot_tcp_ip_config_t;

typedef struct ciot_tcp_config
{
    ciot_tcp_ip_config_t ip;
} ciot_tcp_config_t;

typedef struct ciot_tcp_dhcp_info
{
    ciot_tcp_dhcp_status_t client;
    ciot_tcp_dhcp_status_t server;
} ciot_tcp_dhcp_info_t;

typedef struct ciot_tcp_info
{
    uint8_t mac[6];
    uint8_t ip[4];
} ciot_tcp_info_t;

typedef struct ciot_tcp_status
{
    bool started;
    bool connected;
    uint8_t connection;
    ciot_tcp_dhcp_info_t dhcp;
} ciot_tcp_status_t;

ciot_err_t ciot_tcp_set_config(void *interface, ciot_tcp_config_t *tcp);

ciot_err_t ciot_tcp_config_from_json(CJSON_PARAMETERS(ciot_tcp_config_t));
ciot_err_t ciot_tcp_config_to_json(CJSON_PARAMETERS(ciot_tcp_config_t));
ciot_err_t ciot_tcp_info_from_json(CJSON_PARAMETERS(ciot_tcp_info_t));
ciot_err_t ciot_tcp_info_to_json(CJSON_PARAMETERS(ciot_tcp_info_t));
ciot_err_t ciot_tcp_status_from_json(CJSON_PARAMETERS(ciot_tcp_status_t));
ciot_err_t ciot_tcp_status_to_json(CJSON_PARAMETERS(ciot_tcp_status_t));

#endif  //!__CIOT_TCP__H__