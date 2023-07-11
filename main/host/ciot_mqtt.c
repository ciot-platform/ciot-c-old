/**
 * @file ciot_mqtt.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mongoose.h"

#include "ciot_mqtt.h"
#include "ciot_app.h"
#include "ciot_data_io.h"

#define CIOT_MQTT_TIMEOUT 5000

extern struct mg_mgr mgr;

static void ciot_mqtt_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data);
static void ciot_mqtt_wait_connection(ciot_mqtt_t *mqtt, int timeout);

ciot_err_t ciot_mqtt_connect(ciot_mqtt_t *mqtt)
{
    struct mg_mqtt_opts opts = {
        .clean = true,
        .qos = mqtt->config.connection.qos,
        .version = 4,
        .user = mg_str(mqtt->config.connection.username),
        .pass = mg_str(mqtt->config.connection.password)};
    mqtt->status.state = CIOT_MQTT_STATE_CONNECTING;
    if (mqtt->handle == NULL)
        mqtt->handle = mg_mqtt_connect(&mgr, mqtt->config.connection.host, &opts, ciot_mqtt_event_handler, NULL);
    ciot_mqtt_wait_connection(mqtt, CIOT_MQTT_TIMEOUT);
    return mqtt->status.state == CIOT_MQTT_STATE_CONNECTED ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

ciot_err_t ciot_mqtt_publish(ciot_mqtt_t *mqtt, const char *topic, const char *data, int len, int qos, bool retain)
{
    struct mg_mqtt_opts opts = {
        .topic = mg_str(topic),
        .message = mg_str(data),
        .qos = qos,
        .retain = retain};
    mg_mqtt_pub(mqtt->handle, &opts);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_subscribe(ciot_mqtt_t *mqtt, const char *topic, int qos)
{
    struct mg_mqtt_opts opts = {
        .topic = mg_str(topic),
        .qos = qos};
    mg_mqtt_sub(mqtt->handle, &opts);
    return CIOT_ERR_OK;
}

static void ciot_mqtt_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    ciot_mqtt_t *mqtt = (ciot_mqtt_t*)fn_data;
    ciot_err_t err = CIOT_ERR_OK;
    switch (ev)
    {
    case MG_EV_ERROR:
        printf("MG_EV_ERROR:%d:%s\n", c->id, (char *)ev_data);
        mqtt->status.state = CIOT_MQTT_STATE_ERROR;
        break;
    case MG_EV_MQTT_OPEN:
        printf("MG_EV_MQTT_OPEN\n");
        mqtt->status.connection++;
        mqtt->status.state = CIOT_MQTT_STATE_CONNECTED;
        if (mqtt->on_connection_cb != NULL)
        {
            mqtt->on_connection_cb();
        }
        break;
    case MG_EV_MQTT_MSG:
        printf("MG_EV_MQTT_MSG\n");
        struct mg_mqtt_message *mm = (struct mg_mqtt_message *)ev_data;
        if (strncmp(mm->topic.ptr, mqtt->config.topics.message, mm->topic.len) == 0)
        {
            ciot_data_t msg_data = {
                .data_type = mqtt->config.topics.data_type,
                .buffer.content = (void *)mm->data.ptr,
                .buffer.size = mm->data.len,
            };
            CIOT_ERROR_PRINT(ciot_mqtt_handle_data(mqtt, &msg_data));
        }
        if (mqtt->on_data_cb != NULL)
        {
            mqtt->on_data_cb(mm->topic.ptr, mm->topic.len, mm->data.ptr, mm->data.len);
        }
        break;
    case MG_EV_CLOSE:
        printf("MG_EV_CLOSE\n");
        mqtt->status.state = CIOT_MQTT_STATE_DISCONNECTED;
        mqtt->handle = NULL;
        break;
    default:
        break;
    }
    (void)fn_data;
}

static void ciot_mqtt_wait_connection(ciot_mqtt_t *mqtt, int timeout)
{
    time_t start_time = time(0);
    while (mqtt->status.state == CIOT_MQTT_STATE_CONNECTING)
    {
        time_t current_time = time(0);
        if((start_time + timeout) < current_time)
        {
            return;
        }
    }
}
