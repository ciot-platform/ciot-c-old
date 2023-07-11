/**
 * @file ciot_mqtt.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mqtt_client.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_random.h"

#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "ciot_data_io.h"
#include "ciot_mqtt.h"

#define CIOT_MQTT_EVENT_BIT_CONNECT_DONE BIT0
#define CIOT_MQTT_TIMEOUT 5000

typedef struct ciot_mqtt_idf
{
    EventGroupHandle_t event_group;
} ciot_mqtt_idf_t;

static ciot_mqtt_idf_t this;
static const char *TAG = "ciot_mqtt";

static void ciot_mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

ciot_err_t ciot_mqtt_connect(ciot_mqtt_t *mqtt)
{
    uint8_t mac[6];
    char client_id[32];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    sprintf(client_id, "%x%x%x%x%x%x-%lu", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], esp_random());

    ESP_LOGI(TAG, "config: url:%s port:%d user:<%s> pass:<%s> qos:%d",
        mqtt->config.connection.url,
        mqtt->config.connection.port,
        mqtt->config.connection.username,
        mqtt->config.connection.password,
        mqtt->config.connection.qos
    );

    if(this.event_group == NULL)
    {
        this.event_group = xEventGroupCreate();
    }

    if (mqtt->status.state == CIOT_MQTT_STATE_CONNECTED)
    {
        esp_mqtt_client_disconnect(mqtt->handle);
    }

    const esp_mqtt_client_config_t config = {
        .broker.address.uri = mqtt->config.connection.url,
        .broker.address.port = mqtt->config.connection.port,
        .credentials.username = mqtt->config.connection.username,
        .credentials.authentication.password = mqtt->config.connection.password,
        .credentials.client_id = client_id,
#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach
#endif
    };
    
    mqtt->handle =  esp_mqtt_client_init(&config);
    CIOT_ERROR_RETURN(esp_mqtt_client_register_event(mqtt->handle, ESP_EVENT_ANY_ID, ciot_mqtt_event_handler, mqtt));
    CIOT_ERROR_RETURN(esp_mqtt_client_start(mqtt->handle));
    xEventGroupWaitBits(this.event_group, CIOT_MQTT_EVENT_BIT_CONNECT_DONE, pdTRUE, pdFALSE, CIOT_MQTT_TIMEOUT / portTICK_PERIOD_MS);
    return mqtt->status.state == CIOT_MQTT_STATE_CONNECTED ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

ciot_err_t ciot_mqtt_publish(ciot_mqtt_t *mqtt, const char *topic, const char *data, int len, int qos, bool retain)
{
    int err = esp_mqtt_client_enqueue(mqtt->handle, topic, data, len, qos, retain, false);
    if(err != -1)
    {
        return CIOT_ERR_OK;
    }
    else
    {
        return CIOT_ERR_FAIL;
    }
}

ciot_err_t ciot_mqtt_subscribe(ciot_mqtt_t *mqtt, const char *topic, int qos)
{
    int err = esp_mqtt_client_subscribe(mqtt->handle, topic, qos);
    if(err != -1)
    {
        return CIOT_ERR_OK;
    }
    else
    {
        return CIOT_ERR_FAIL;
    }
}

static void ciot_mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ciot_mqtt_t *mqtt = (ciot_mqtt_t*)handler_args;
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        mqtt->status.state = CIOT_MQTT_STATE_ERROR;
        break;
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        mqtt->status.connection++;
        mqtt->status.state = CIOT_MQTT_STATE_CONNECTED;
        if (mqtt->on_connection_cb != NULL)
        {
            mqtt->on_connection_cb();
        }
        xEventGroupSetBits(this.event_group, CIOT_MQTT_EVENT_BIT_CONNECT_DONE);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqtt->status.state = CIOT_MQTT_STATE_DISCONNECTED;
        xEventGroupSetBits(this.event_group, CIOT_MQTT_EVENT_BIT_CONNECT_DONE);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d, msg_size=%d", event->msg_id, event->data_len);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        if (strncmp(event->topic, mqtt->config.topics.message, event->topic_len) == 0)
        {
            ESP_LOGI(TAG, "CIOT APP Processing msg...");
            ciot_data_t msg_data = {
                .data_type = mqtt->config.topics.data_type,
                .buffer.content = (void *)event->data,
                .buffer.size = event->data_len,
            };
            CIOT_ERROR_PRINT(ciot_mqtt_handle_data(mqtt, &msg_data));
        }
        if (mqtt->on_data_cb != NULL)
        {
            mqtt->on_data_cb(event->topic, event->topic_len, event->data, event->data_len);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
