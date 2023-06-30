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

#include "ciot_config.h"
#include "ciot_wifi.h"

#define CIOT_WIFI_EVENT_BIT_CONFIG_DONE BIT0
#define CIOT_WIFI_EVENT_BIT_SCAN_DONE BIT1
#define CIOT_WIFI_SCAN_TIMEOUT 5000

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
static ciot_err_t ciot_wifi_scan(ciot_wifi_scan_result_t *scan_result);
static void ciot_wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static ciot_err_t ciot_wifi_get_scan_result(ciot_wifi_scan_result_t *scan_result);

ciot_err_t ciot_wifi_set_config(ciot_wifi_config_t *conf)
{
    ESP_LOGI(TAG, "config: mode:%d ssid:<%s> password:<%s> timeout:%d", conf->mode, conf->ssid, conf->password, conf->timeout);
    wifi_interface_t wifi_interface = conf->mode;
    wifi_config_t wifi_conf = {0};
    wifi_mode_t wifi_mode = ciot_wifi_get_mode(wifi_interface);

    memcpy(wifi_conf.ap.ssid, conf->ssid, sizeof(wifi_conf.ap.ssid));
    memcpy(wifi_conf.ap.password, conf->password, sizeof(wifi_conf.ap.password));
    memcpy(&wifi[conf->mode].config, conf, sizeof(wifi[conf->mode].config));
    ciot_wifi_init();

    if (conf->mode == CIOT_WIFI_IF_AP)
    {
        wifi_conf.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
        wifi_conf.ap.max_connection = 1;
    }
    else if(wifi[CIOT_WIFI_IF_STA].status.data.tcp.state == CIOT_TCP_STATE_CONNECTED)
    {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(event_group, CIOT_WIFI_EVENT_BIT_CONFIG_DONE, pdTRUE, pdFALSE, conf->timeout / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_mode));
    ESP_ERROR_CHECK(esp_wifi_set_config(wifi_interface, &wifi_conf));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (conf->mode == CIOT_WIFI_IF_STA && conf->timeout != 0)
    {
        if(wifi[CIOT_WIFI_IF_STA].status.data.tcp.state == CIOT_TCP_STATE_STARTED)
        {
            ESP_ERROR_CHECK(esp_wifi_connect());
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.state = CIOT_TCP_STATE_CONNECTING;
        }
        xEventGroupWaitBits(event_group, CIOT_WIFI_EVENT_BIT_CONFIG_DONE, pdTRUE, pdFALSE, conf->timeout / portTICK_PERIOD_MS);
        return wifi[CIOT_WIFI_IF_STA].status.data.tcp.state == CIOT_TCP_STATE_CONNECTED ? CIOT_ERR_OK : CIOT_ERR_FAIL;
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_get_config(ciot_wifi_interface_t interface, ciot_wifi_config_t *config)
{
    if (interface != CIOT_WIFI_IF_AP && interface != CIOT_WIFI_IF_STA)
    {
        return CIOT_ERR_INVALID_TYPE;
    }
    *config = wifi[interface].config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_get_status(ciot_wifi_interface_t interface, ciot_wifi_status_t *status)
{
    if (interface != CIOT_WIFI_IF_AP && interface != CIOT_WIFI_IF_STA)
    {
        return CIOT_ERR_INVALID_TYPE;
    }

    if (interface == CIOT_WIFI_IF_STA)
    {
        wifi_ap_record_t ap;

        esp_err_t err = esp_wifi_sta_get_ap_info(&ap);
        if(err)
            return err;

        wifi[interface].status.data.sta.sta.authmode = ap.authmode;
        wifi[interface].status.data.sta.sta.rssi = ap.rssi;
        memcpy(wifi[interface].status.data.sta.sta.bssid, ap.bssid, 6);
        memcpy(wifi[interface].status.data.sta.sta.ssid, ap.ssid, 32);
    }

    *status = wifi[interface].status;
    status->interface = interface;

    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_get_info(ciot_wifi_interface_t interface, ciot_wifi_info_t *info)
{
    if (interface != CIOT_WIFI_IF_AP && interface != CIOT_WIFI_IF_STA)
    {
        return CIOT_ERR_INVALID_TYPE;
    }
    memcpy(info, &wifi[interface].info, sizeof(*info));
    info->available = CIOT_CONFIG_FEATURE_WIFI;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_wifi_process_request(ciot_wifi_request_t request, ciot_wifi_scan_result_t *scan_result)
{
    switch (request)
    {
    case CIOT_WIFI_REQUEST_SCAN:
        return ciot_wifi_scan(scan_result);
    default:    
        return CIOT_ERR_INVALID_REQUEST;
    }
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

        wifi[CIOT_WIFI_IF_STA].netif = esp_netif_create_default_wifi_sta();
        wifi[CIOT_WIFI_IF_AP].netif = esp_netif_create_default_wifi_ap();

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
        return wifi[CIOT_WIFI_IF_AP].status.data.tcp.state >= CIOT_TCP_STATE_STARTED ? WIFI_MODE_APSTA : WIFI_MODE_STA;
    case WIFI_IF_AP:
        return wifi[CIOT_WIFI_IF_STA].status.data.tcp.state >= CIOT_TCP_STATE_STARTED ? WIFI_MODE_APSTA : WIFI_MODE_AP;
    default:
        return WIFI_MODE_NULL;
    }
}

static ciot_err_t ciot_wifi_scan(ciot_wifi_scan_result_t *scan_result)
{
    esp_err_t err = CIOT_ERR_OK;

    if(wifi[CIOT_WIFI_IF_STA].status.data.tcp.state <= CIOT_TCP_STATE_STOPPED)
    {
        ciot_wifi_init();

        wifi_mode_t wifi_mode = ciot_wifi_get_mode(CIOT_WIFI_IF_STA);
        err = esp_wifi_set_mode(wifi_mode);
        if(err != CIOT_ERR_OK)
            return err;
        
        err = esp_wifi_start();
        if(err != CIOT_ERR_OK)
            return err;
    }
    
    err = esp_wifi_scan_start(NULL, false);
    if(err != CIOT_ERR_OK)
        return err;
    
    wifi[CIOT_WIFI_IF_AP].status.data.sta.scan = CIOT_WIFI_SCAN_STATE_SCANNING;
    xEventGroupWaitBits(event_group, CIOT_WIFI_EVENT_BIT_SCAN_DONE, pdTRUE, pdFALSE, CIOT_WIFI_SCAN_TIMEOUT / portTICK_PERIOD_MS);
    
    return ciot_wifi_get_scan_result(scan_result);
}

static void ciot_wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_SCAN_DONE:
            ESP_LOGI(TAG, "WIFI_EVENT_SCAN_DONE");
            xEventGroupSetBits(event_group, CIOT_WIFI_EVENT_BIT_SCAN_DONE);
            wifi[CIOT_WIFI_IF_AP].status.data.sta.scan = CIOT_WIFI_SCAN_STATE_SCANNED;
            break;
        case WIFI_EVENT_AP_START:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.state = CIOT_TCP_STATE_STARTED;
            ESP_ERROR_CHECK_WITHOUT_ABORT(ciot_tcp_set_config(wifi[CIOT_WIFI_IF_AP].netif, &wifi[CIOT_WIFI_IF_AP].config.tcp));
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_mac(WIFI_IF_AP, wifi[CIOT_WIFI_IF_AP].info.tcp.mac));
            break;
        case WIFI_EVENT_AP_STOP:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STOP");
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.state = CIOT_TCP_STATE_STOPPED;
            break;
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STACONNECTED");
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.state = CIOT_TCP_STATE_CONNECTED;
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.connection++;
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_AP_STADISCONNECTED");
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.state = CIOT_TCP_STATE_STARTED;
            break;
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_get_mac(WIFI_IF_STA, wifi[CIOT_WIFI_IF_STA].info.tcp.mac));
            ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_connect());
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.state = CIOT_TCP_STATE_CONNECTING;
            break;
        case WIFI_EVENT_STA_STOP:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_STOP");
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.state = CIOT_TCP_STATE_STOPPED;
            break;
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.state = CIOT_TCP_STATE_CONNECTED;
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.connection++;
            ESP_ERROR_CHECK_WITHOUT_ABORT(ciot_tcp_set_config(wifi[CIOT_WIFI_IF_STA].netif, &wifi[CIOT_WIFI_IF_STA].config.tcp));
            xEventGroupSetBits(event_group, CIOT_WIFI_EVENT_BIT_CONFIG_DONE);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.state = CIOT_TCP_STATE_STARTED;
            xEventGroupSetBits(event_group, CIOT_WIFI_EVENT_BIT_CONFIG_DONE);
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
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.ip[0] = ip4_addr1(&event->ip_info.ip);
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.ip[1] = ip4_addr2(&event->ip_info.ip);
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.ip[2] = ip4_addr3(&event->ip_info.ip);
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.ip[3] = ip4_addr4(&event->ip_info.ip);

            esp_netif_dhcp_status_t status;
            esp_netif_dhcpc_get_status(wifi[CIOT_WIFI_IF_STA].netif, &status);
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.dhcp.client = status;
            esp_netif_dhcps_get_status(wifi[CIOT_WIFI_IF_STA].netif, &status);
            wifi[CIOT_WIFI_IF_STA].status.data.tcp.dhcp.server = status;
            break;
        }
        case IP_EVENT_AP_STAIPASSIGNED:
        {
            ESP_LOGI(TAG, "IP_EVENT_AP_STAIPASSIGNED");
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.ip[0] = ip4_addr1(&event->ip_info.ip);
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.ip[1] = ip4_addr2(&event->ip_info.ip);
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.ip[2] = ip4_addr3(&event->ip_info.ip);
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.ip[3] = ip4_addr4(&event->ip_info.ip);

            esp_netif_dhcp_status_t status;
            esp_netif_dhcpc_get_status(wifi[CIOT_WIFI_IF_AP].netif, &status);
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.dhcp.client = status;
            esp_netif_dhcps_get_status(wifi[CIOT_WIFI_IF_AP].netif, &status);
            wifi[CIOT_WIFI_IF_AP].status.data.tcp.dhcp.server = status;
            break;
        }
        default:
            break;
        }
    }
}

static ciot_err_t ciot_wifi_get_scan_result(ciot_wifi_scan_result_t *scan_result)
{
    wifi_ap_record_t *wifi_ap_record;
    uint16_t size = 0;

    esp_err_t err = esp_wifi_scan_get_ap_num(&size);
    if(err != ESP_OK)
    {
        scan_result->state = CIOT_WIFI_SCAN_STATE_ERROR;
        return err;
    }

    if(size > 0)
    {
        uint16_t max_size = (sizeof(scan_result->result) / sizeof(ciot_wifi_ap_info_t));
        scan_result->size = size > max_size ? max_size : size;
        wifi_ap_record = calloc(scan_result->size, sizeof(wifi_ap_record_t));
        if(wifi_ap_record != NULL)
        {
            err = esp_wifi_scan_get_ap_records(&scan_result->size, wifi_ap_record);
            if(err == ESP_OK)
            {
                if(scan_result->result != NULL)
                {
                    for (size_t i = 0; i < scan_result->size; i++)
                    {
                        memcpy(scan_result->result[i].bssid, wifi_ap_record[i].bssid, 6);
                        memcpy(scan_result->result[i].ssid, wifi_ap_record[i].ssid, 32);
                        scan_result->result[i].authmode = wifi_ap_record[i].authmode;
                        scan_result->result[i].rssi = wifi_ap_record[i].rssi;
                    }
                }
                else
                {
                    scan_result->state = CIOT_WIFI_SCAN_STATE_ERROR;
                }
            }
        }
        else
        {
            scan_result->state = CIOT_WIFI_SCAN_STATE_ERROR;
        }
        free(wifi_ap_record);
    }
    return err;
}