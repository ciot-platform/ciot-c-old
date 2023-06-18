/**
 * @file ciot_tcp_dto.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_TCP_DTO__H__
#define __CIOT_TCP_DTO__H__

#include "ciot_tcp.h"

static ciot_err_t ciot_tcp_ip_config_from_json(CJSON_PARAMETERS(ciot_tcp_ip_config_t));
static ciot_err_t ciot_tcp_ip_config_to_json(CJSON_PARAMETERS(ciot_tcp_ip_config_t));
static ciot_err_t ciot_tcp_dhcp_info_from_json(CJSON_PARAMETERS(ciot_tcp_dhcp_info_t));
static ciot_err_t ciot_tcp_dhcp_info_to_json(CJSON_PARAMETERS(ciot_tcp_dhcp_info_t));

ciot_err_t ciot_tcp_config_from_json(CJSON_PARAMETERS(ciot_tcp_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_OBJ(ip, ciot_tcp_ip_config_from_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_config_to_json(CJSON_PARAMETERS(ciot_tcp_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ(ip, ciot_tcp_ip_config_to_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_info_from_json(CJSON_PARAMETERS(ciot_tcp_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER_ARRAY(mac);
    CJSON_GET_NUMBER_ARRAY(ip);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_info_to_json(CJSON_PARAMETERS(ciot_tcp_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER_ARRAY(mac);
    CJSON_ADD_NUMBER_ARRAY(ip);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_status_from_json(CJSON_PARAMETERS(ciot_tcp_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_BOOLEAN(started);
    CJSON_GET_BOOLEAN(connected);
    CJSON_GET_OBJ(dhcp, ciot_tcp_dhcp_info_from_json);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_tcp_status_to_json(CJSON_PARAMETERS(ciot_tcp_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_BOOLEAN(started);
    CJSON_ADD_BOOLEAN(connected);
    CJSON_ADD_OBJ(dhcp, ciot_tcp_dhcp_info_to_json);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_tcp_ip_config_from_json(CJSON_PARAMETERS(ciot_tcp_ip_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(dhcp);
    CJSON_GET_NUMBER_ARRAY(address);
    CJSON_GET_NUMBER_ARRAY(gateway);
    CJSON_GET_NUMBER_ARRAY(mask);
    CJSON_GET_NUMBER_ARRAY(dns);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_tcp_ip_config_to_json(CJSON_PARAMETERS(ciot_tcp_ip_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(dhcp);
    CJSON_ADD_NUMBER_ARRAY(address);
    CJSON_ADD_NUMBER_ARRAY(gateway);
    CJSON_ADD_NUMBER_ARRAY(mask);
    CJSON_ADD_NUMBER_ARRAY(dns);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_tcp_dhcp_info_from_json(CJSON_PARAMETERS(ciot_tcp_dhcp_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(client);
    CJSON_GET_NUMBER(server);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_tcp_dhcp_info_to_json(CJSON_PARAMETERS(ciot_tcp_dhcp_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(client);
    CJSON_ADD_NUMBER(server);
    return CIOT_ERR_OK;
}

#endif  //!__CIOT_TCP_DTO__H__