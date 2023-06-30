/**
 * @file ciot_tcp.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "esp_netif.h"
#include "esp_log.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"

#include "ciot_err.h"
#include "ciot_tcp.h"

static const char *TAG = "ciot_tcp";

static ciot_err_t ciot_tcp_set_dhcp_config(void *interface, ciot_tcp_dhcp_config_t dhcp);
static ciot_err_t ciot_tcp_set_ip_config(void *interface, ciot_tcp_ip_config_t *ip);

ciot_err_t ciot_tcp_set_config(void *interface, ciot_tcp_config_t *tcp)
{
    esp_err_t err = ciot_tcp_set_dhcp_config(interface, tcp->ip.dhcp);
    if (err)
        return err;

    err = ciot_tcp_set_ip_config(interface, &tcp->ip);
    if (err)
        return err;

    return err;
}

static ciot_err_t ciot_tcp_set_dhcp_config(void *interface, ciot_tcp_dhcp_config_t dhcp)
{
    esp_netif_t *netif = (esp_netif_t *)interface;

    esp_err_t err = CIOT_ERR_OK;
    esp_netif_dhcp_status_t dhcpc;
    esp_netif_dhcp_status_t dhcps;
    esp_netif_dhcpc_get_status(netif, &dhcpc);
    esp_netif_dhcps_get_status(netif, &dhcps);

    switch (dhcp)
    {
    case CIOT_TCP_DHCP_CONFIG_NO_CHANGE:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CONFIG_NO_CHANGE");
        return CIOT_ERR_OK;
    case CIOT_TCP_DHCP_CONFIG_CLIENT:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CONFIG_CLIENT");
        if (dhcps != ESP_NETIF_DHCP_STOPPED)
        {
            err = esp_netif_dhcps_stop(netif);
            if (err)
                return err;
        }
        if (dhcpc != ESP_NETIF_DHCP_STARTED)
        {
            err = esp_netif_dhcpc_stop(netif);
            if (err)
                return err;
        }
        return err;
    case CIOT_TCP_DHCP_CONFIG_SERVER:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CONFIG_SERVER");
        if (dhcpc != ESP_NETIF_DHCP_STOPPED)
        {
            err = esp_netif_dhcpc_stop(netif);
            if (err)
                return err;
        }
        if (dhcps != ESP_NETIF_DHCP_STARTED)
        {
            err = esp_netif_dhcps_start(netif);
            if (err)
                return err;
        }
        return err;
    case CIOT_TCP_DHCP_CONFIG_DISABLED:
        ESP_LOGI(TAG, "CIOT_TCP_DHCP_CONFIG_DISABLED");
        if (dhcpc != ESP_NETIF_DHCP_STOPPED)
        {
            err = esp_netif_dhcpc_stop(netif);
            if (err)
                return err;
        }
        if (dhcps != ESP_NETIF_DHCP_STOPPED)
        {
            err = esp_netif_dhcps_stop(netif);
            if (err)
                return err;
        }
        return err;
    default:
        return CIOT_ERR_INVALID_ARG;
    }
}

static ciot_err_t ciot_tcp_set_ip_config(void *interface, ciot_tcp_ip_config_t *ip)
{
    if(ip->dhcp == CIOT_TCP_DHCP_CONFIG_DISABLED)
    {
        ESP_LOGI(TAG, "config:ip: ip:%d.%d.%d.%d gw:%d.%d.%d.%d mask:%d.%d.%d.%d dns:%d.%d.%d.%d",
                ip->address[0], ip->address[1], ip->address[2], ip->address[3],
                ip->gateway[0], ip->gateway[1], ip->gateway[2], ip->gateway[3],
                ip->mask[0], ip->mask[1], ip->mask[2], ip->mask[3],
                ip->dns[0], ip->dns[1], ip->dns[2], ip->dns[3]);

        char address[16];
        char gateway[16];
        char mask[16];
        char dns[16];

        esp_netif_t *netif = (esp_netif_t *)interface;
        esp_netif_ip_info_t ip_info;
        esp_netif_dns_info_t dns_info;
        esp_err_t err;

        sprintf(address, "%d.%d.%d.%d", ip->address[0], ip->address[1], ip->address[2], ip->address[3]);
        sprintf(gateway, "%d.%d.%d.%d", ip->gateway[0], ip->gateway[1], ip->gateway[2], ip->gateway[3]);
        sprintf(mask, "%d.%d.%d.%d", ip->mask[0], ip->mask[1], ip->mask[2], ip->mask[3]);
        sprintf(dns, "%d.%d.%d.%d", ip->dns[0], ip->dns[1], ip->dns[2], ip->dns[3]);

        ip_info.ip.addr = ipaddr_addr(address);
        ip_info.gw.addr = ipaddr_addr(gateway);
        ip_info.netmask.addr = ipaddr_addr(mask);
        dns_info.ip.u_addr.ip4.addr = ipaddr_addr(mask);
        dns_info.ip.type = IPADDR_TYPE_V4;

        err = esp_netif_set_ip_info(netif, &ip_info);
        if (err)
            return err;

        return esp_netif_set_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
    }
    return CIOT_ERR_OK;
}
