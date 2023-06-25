/**
 * @file ciot_wifi.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <string.h>

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "lwip/ip4_addr.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "ciot_wifi.h"

#define CIOT_WIFI_EVENT_BIT_DONE BIT0

typedef struct ciot_wifi
{
    esp_netif_t *netif;
    ciot_wifi_config_t config;
    ciot_wifi_info_t info;
    ciot_wifi_status_t status;
} ciot_wifi_t;

static ciot_wifi_t wifi[2];
static EventGroupHandle_t event_group;
static const char *TAG = "ciot_wifi";

static void ciot_wifi_init(void);
static wifi_mode_t ciot_wifi_get_mode(wifi_interface_t interface);
static void ciot_wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

ciot_err_t ciot_wifi_set_config(ciot_wifi_config_t *conf)
{
    ESP_LOGI(TAG, "config: mode:%d ssid:<%s> password:<%s> timeout:%d", conf->mode, conf->ssid, conf->password, conf->timeout);
    wifi_interface_t wifi_interface = conf->mode;
    wifi_config_t wifi_conf = {0};
    wifi_mode_t wifi_mode = ciot_wifi_get_mode(wifi_interface);

    memcpy(&wifi_conf, conf, sizeof(*conf));
    memcpy(&wifi[conf->mode].config, conf, sizeof(wifi[conf->mode].config));
    ciot_wifi_init();

    if (conf->mode == CIOT_WIFI_MODE_AP)
    {
        wifi_conf.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        wifi_conf.ap.max_connection = 1;
    }
    else if(wifi[CIOT_WIFI_MODE_STA].status.tcp.state == CIOT_TCP_STATE_CONNECTED)
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(event_group, CIOT_WIFI_EVENT_BIT_DONE, pdTRUE, pdFALSE, conf->timeout / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_mode));
    ESP_ERROR_CHECK(esp_wifi_set_config(wifi_interface, &wifi_conf));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (conf->mode == CIOT_WIFI_MODE_STA && conf->timeout != 0)
    {
        if(wifi[CIOT_WIFI_MODE_STA].status.tcp.state == CIOT_TCP_STATE_STARTED)
        {
            ESP_ERROR_CHECK(esp_wifi_connect());
            wifi[CIOT_WIFI_MODE_STA].status.tcp.state = CIOT_TCP_STATE_CONNECTING;
        }
        xEventGroupWaitBits(event_group, CIOT_WIFI_EVENT_BIT_DONE, pdTRUE, pdFALSE, conf->timeout / portTICK_PERIOD_MS);
        return wifi[CIOT_WIFI_MODE_STA].status.tcp.state == CIOT_TCP_STATE_CONNECTED ? CIOT_ERR_OK : CIOT_ERR_FAIL;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_get_config(ciot_wifi_mode_t type, ciot_wifi_config_t *config)
{
    if (type != CIOT_WIFI_MODE_AP && type != CIOT_WIFI_MODE_STA)
    {
        return CIOT_ERR_INVALID_TYPE;
    }
    *config = wifi[type].config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_get_status(ciot_wifi_mode_t type, ciot_wifi_status_t *status)
{
    if (type != CIOT_WIFI_MODE_AP && type != CIOT_WIFI_MODE_STA)
    {
        return CIOT_ERR_INVALID_TYPE;
    }
    *status = wifi[type].status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_get_info(ciot_wifi_mode_t type, ciot_wifi_info_t *info)
{
    if (type != CIOT_WIFI_MODE_AP && type != CIOT_WIFI_MODE_STA)
    {
        return CIOT_ERR_INVALID_TYPE;
    }
    memcpy(info, &wifi[type].info, sizeof(*info));
    return CIOT_ERR_OK;
}

static void ciot_wifi_init(void)
{
    static bool init = false;
    if (init == false)
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&config));

        wifi[CIOT_WIFI_MODE_STA].netif = esp_netif_create_default_wifi_sta();
        wifi[CIOT_WIFI_MODE_AP].netif = esp_netif_create_default_wifi_ap();

        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &ciot_wifi_event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ciot_wifi_event_handler, NULL));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

        event_group = xEventGroupCreate();

        init = true;
    }
}

static wifi_mode_t ciot_wifi_get_mode(wifi_interface_t interface)
{
    switch (interface)
    {
    case WIFI_IF_STA:
        return wifi[CIOT_WIFI_MODE_AP].status.tcp.state >= CIOT_TCP_STATE_STARTED ? WIFI_MODE_APSTA : WIFI_MODE_STA;
    case WIFI_IF_AP:
        return wifi[CIOT_WIFI_MODE_STA].status.tcp.state >= CIOT_TCP_STATE_STARTED ? WIFI_MODE_APSTA : WIFI_MODE_AP;
    default:
        return WIFI_MODE_NULL;
    }
}

static void ciot_wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_SCAN_DONE:
            ESP_LOGI(TAG, "WIFI_EVENT_SCAN_DONE");
            break;
        case WIFI_EVENT_AP_START:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
            wifi[CIOT_WIFI_MODE_AP].status.tcp.state = CIOT_TCP_STATE_STARTED;
            ESP_ERROR_CHECK_WITHOUT_ABORT(ciot_tcp_set_config(wifi[CIOT_WIFI_MODE_AP].netif, &wifi[CIOT_WIFI_MODE_AP].config.tcp));
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_mac(WIFI_IF_AP, wifi[CIOT_WIFI_MODE_AP].info.tcp.mac));
            break;
        case WIFI_EVENT_AP_STOP:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
            wifi[CIOT_WIFI_MODE_AP].status.tcp.state = CIOT_TCP_STATE_STOPPED;
            break;
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
            wifi[CIOT_WIFI_MODE_AP].status.tcp.state = CIOT_TCP_STATE_CONNECTED;
            wifi[CIOT_WIFI_MODE_STA].status.tcp.connection++;
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
            wifi[CIOT_WIFI_MODE_AP].status.tcp.state = CIOT_TCP_STATE_STARTED;
            break;
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_mac(WIFI_IF_STA, wifi[CIOT_WIFI_MODE_STA].info.tcp.mac));
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_connect());
            wifi[CIOT_WIFI_MODE_STA].status.tcp.state = CIOT_TCP_STATE_CONNECTING;
            break;
        case WIFI_EVENT_STA_STOP:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_STOP");
            wifi[CIOT_WIFI_MODE_STA].status.tcp.state = CIOT_TCP_STATE_STOPPED;
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
            wifi[CIOT_WIFI_MODE_STA].status.tcp.state = CIOT_TCP_STATE_CONNECTED;
            wifi[CIOT_WIFI_MODE_STA].status.tcp.connection++;
            ciot_tcp_set_config(wifi[CIOT_WIFI_MODE_STA].netif, &wifi[CIOT_WIFI_MODE_STA].config.tcp);
            xEventGroupSetBits(event_group, CIOT_WIFI_EVENT_BIT_DONE);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            wifi[CIOT_WIFI_MODE_STA].status.tcp.state = CIOT_TCP_STATE_STARTED;
            xEventGroupSetBits(event_group, CIOT_WIFI_EVENT_BIT_DONE);
            break;
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
        {
            ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            wifi[CIOT_WIFI_MODE_STA].info.tcp.ip[0] = ip4_addr1(&event->ip_info.ip);
            wifi[CIOT_WIFI_MODE_STA].info.tcp.ip[1] = ip4_addr2(&event->ip_info.ip);
            wifi[CIOT_WIFI_MODE_STA].info.tcp.ip[2] = ip4_addr3(&event->ip_info.ip);
            wifi[CIOT_WIFI_MODE_STA].info.tcp.ip[3] = ip4_addr4(&event->ip_info.ip);

            esp_netif_dhcp_status_t status;
            esp_netif_dhcpc_get_status(wifi[CIOT_WIFI_MODE_STA].netif, &status);
            wifi[CIOT_WIFI_MODE_STA].status.tcp.dhcp.client = status;
            esp_netif_dhcps_get_status(wifi[CIOT_WIFI_MODE_STA].netif, &status);
            wifi[CIOT_WIFI_MODE_STA].status.tcp.dhcp.server = status;
            break;
        }
        case IP_EVENT_AP_STAIPASSIGNED:
        {
            ESP_LOGI(TAG, "IP_EVENT_AP_STAIPASSIGNED");
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            wifi[CIOT_WIFI_MODE_AP].info.tcp.ip[0] = ip4_addr1(&event->ip_info.ip);
            wifi[CIOT_WIFI_MODE_AP].info.tcp.ip[1] = ip4_addr2(&event->ip_info.ip);
            wifi[CIOT_WIFI_MODE_AP].info.tcp.ip[2] = ip4_addr3(&event->ip_info.ip);
            wifi[CIOT_WIFI_MODE_AP].info.tcp.ip[3] = ip4_addr4(&event->ip_info.ip);

            esp_netif_dhcp_status_t status;
            esp_netif_dhcpc_get_status(wifi[CIOT_WIFI_MODE_AP].netif, &status);
            wifi[CIOT_WIFI_MODE_AP].status.tcp.dhcp.client = status;
            esp_netif_dhcps_get_status(wifi[CIOT_WIFI_MODE_AP].netif, &status);
            wifi[CIOT_WIFI_MODE_AP].status.tcp.dhcp.server = status;
            break;
        }
        default:
            break;
        }
    }
}
