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

typedef struct ciot_mqtt
{
    struct mg_connection *s_conn;
    struct mg_mgr mgr;
    ciot_mqtt_config_t config;
    ciot_mqtt_info_t info;
    ciot_mqtt_status_t status;
    ciot_mqtt_on_data_cb_t *on_data_cb;
    ciot_mqtt_on_connection_cb_t *on_connection_cb;
} ciot_mqtt_t;

static ciot_mqtt_t this;

static void ciot_mqtt_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

ciot_err_t ciot_mqtt_set_config(ciot_mqtt_config_t *conf)
{
    mg_mgr_init(&this.mgr);
    memcpy(&this.config, conf, sizeof(this.config));
    struct mg_mqtt_opts opts = {
        .clean = true,
        .qos = this.config.qos,
        .version = 4,
        .user = mg_str(this.config.username),
        .pass = mg_str(this.config.password)
    };
    if(this.s_conn == NULL) this.s_conn = mg_mqtt_connect(&this.mgr, this.config.host, &opts, ciot_mqtt_event_handler, NULL);
    
    while (true) mg_mgr_poll(&this.mgr, 1000);
    mg_mgr_free(&this.mgr);
    
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_get_config(ciot_mqtt_config_t *config)
{
    *config = this.config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_get_status(ciot_mqtt_status_t *status)
{
    *status = this.status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_get_info(ciot_mqtt_info_t *info)
{
    *info = this.info;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_publish(const char *topic, const char *data, int len, int qos, bool retain)
{
    struct mg_mqtt_opts opts = {
        .topic = mg_str(topic),
        .message = mg_str(data),
        .qos = qos,
        .retain = retain
    };
    mg_mqtt_pub(this.s_conn, &opts);
}

ciot_err_t ciot_mqtt_subscribe(const char *topic, int qos)
{
    struct mg_mqtt_opts opts = {
        .topic = mg_str(topic),
        .qos = qos
    };
    mg_mqtt_sub(this.s_conn, &opts);
}

ciot_err_t ciot_mqtt_on_data(ciot_mqtt_on_data_cb_t on_data_cb)
{
    this.on_data_cb = on_data_cb;
}

ciot_err_t ciot_mqtt_on_connection(ciot_mqtt_on_connection_cb_t on_connection_cb)
{
    this.on_connection_cb = on_connection_cb;
}

static void ciot_mqtt_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    switch (ev)
    {
    case MG_EV_OPEN:
        printf("MG_EV_OPEN\n");
        break;
    case MG_EV_ERROR:
        printf("MG_EV_ERROR:%d:%s\n", c->id, (char*)ev_data);
        break;
    case MG_EV_CONNECT:
        printf("MG_EV_CONNECT\n");
        break;
    case MG_EV_MQTT_OPEN:
        printf("MG_EV_MQTT_OPEN\n");
        if(this.on_connection_cb != NULL)
        {
            this.on_connection_cb();
        }
        break;
    case MG_EV_MQTT_MSG:
        printf("MG_EV_MQTT_MSG\n");
        if(this.on_data_cb != NULL)
        {
            struct mg_mqtt_message *mm = (struct mg_mqtt_message *) ev_data;
            this.on_data_cb(mm->topic.ptr, mm->topic.len, mm->data.ptr, mm->data.len);
        }
        break;
    case MG_EV_CLOSE:
        printf("MG_EV_CLOSE\n");
        this.s_conn = NULL;
        break;
    default:
        break;
    }
    (void) fn_data;
}
