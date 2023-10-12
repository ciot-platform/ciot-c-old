/**
 * @file ciot_mqtt.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "stdlib.h"
#include "ciot_mqttc.h"
#include "mongoose.h"

struct ciot_mqtt
{
    ciot_iface_t iface;
    ciot_mqttc_cfg_t cfg;
    ciot_mqttc_status_t status;
    struct mg_mgr *mgr;
    struct mg_connection *connection;
};

static void ciot_mqttc_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

ciot_mqttc_t ciot_mqttc_new(void *handle)
{
    ciot_mqttc_t this = calloc(1, sizeof(struct ciot_mqtt));
    this->iface.base.ptr = this;
    this->iface.base.start = (ciot_iface_start_fn *)ciot_mqttc_start;
    this->iface.base.stop = (ciot_iface_stop_fn *)ciot_mqttc_stop;
    this->iface.base.process_req = (ciot_iface_process_req_fn *)ciot_mqttc_process_req;
    this->iface.base.send_data = (ciot_iface_send_data_fn *)ciot_mqttc_send_data;
    this->iface.base.cfg.ptr = &this->cfg;
    this->iface.base.cfg.size = sizeof(this->cfg);
    this->iface.base.status.ptr = &this->status;
    this->iface.base.status.size = sizeof(this->status);
    this->iface.base.type = CIOT_IFACE_TYPE_MQTT;
    this->mgr = handle;
    return this;
}

ciot_err_t ciot_mqttc_start(ciot_mqttc_t this, ciot_mqttc_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(cfg);
    struct mg_mqtt_opts opts = {0};
    memcpy(&this->cfg, cfg, sizeof(this->cfg));
    opts.user = mg_str(this->cfg.user);
    opts.pass = mg_str(this->cfg.pass);
    opts.qos = this->cfg.qos;
    opts.version = 4;
    opts.clean = true;
    this->status.state = CIOT_MQTT_STATE_CONNECTING;
    if (this->connection == NULL)
    {
        this->connection = mg_mqtt_connect(this->mgr, this->cfg.url, &opts, ciot_mqttc_event_handler, this);
    }
}

ciot_err_t ciot_mqttc_stop(ciot_mqttc_t this)
{
    CIOT_ERR_NULL_CHECK(this);
    mg_mqtt_disconnect(this->connection, NULL);
    return CIOT_OK;
}

ciot_err_t ciot_mqttc_process_req(ciot_mqttc_t this, ciot_mqttc_req_t *req)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(req);

    switch (req->id)
    {
    case CIOT_MQTT_REQ_PUBLISH:
        return ciot_mqttc_publish(this, &req->data.publish);
    case CIOT_MQTT_REQ_SUBSCRIBE:
        return ciot_mqttc_subscribe(this, &req->data.subscribe);
        break;
    default:
        break;
    }
}

ciot_err_t ciot_mqttc_send_data(ciot_mqttc_t this, uint8_t *data, int size)
{
    ciot_mqttc_req_publish_t req = {0};
    memcpy(req.topic, this->cfg.topics.d2b, sizeof(req.topic));
    memcpy(req.msg, data, size);
    req.qos = this->cfg.qos;
    req.size = size;
    ciot_mqttc_publish(this, &req);
}

ciot_err_t ciot_mqttc_publish(ciot_mqttc_t this, ciot_mqttc_req_publish_t *req)
{
    if (req->size < CIOT_CONFIG_MESSAGE_LEN)
    {
        struct mg_mqtt_opts opts = {0};
        struct mg_str msg = {0};
        msg.ptr = (const char *)req->msg;
        msg.len = req->size;
        opts.topic = mg_str(req->topic);
        opts.message = msg;
        opts.qos = req->qos;
        opts.retain = false;
        mg_mqtt_pub(this->connection, &opts);
        return CIOT_OK;
    }
    else
    {
        return CIOT_ERR_OVERFLOW;
    }
}

ciot_err_t ciot_mqttc_subscribe(ciot_mqttc_t this, ciot_mqttc_req_subscribe_t *req)
{
    struct mg_mqtt_opts opts = {0};
    opts.topic = mg_str(req->topic);
    opts.qos = req->qos;
    mg_mqtt_sub(this->connection, &opts);
    return CIOT_OK;
}

static void ciot_mqttc_on_msg(ciot_mqttc_t this, struct mg_connection *c, struct mg_mqtt_message *mm)
{
    ciot_iface_event_t event = {0};
    if (strncmp(mm->topic.ptr, this->cfg.topics.b2d, CIOT_MQTT_TOPIC_LEN) == 0)
    {
        event.id = CIOT_IFACE_EVENT_REQUEST;
        event.size = mm->data.len;
        memcpy(&event.msg, mm->data.ptr, mm->data.len);
    }
    else
    {
        event.id = CIOT_MQTT_EVENT_DATA;
        event.msg.id = this->iface.id;
        event.msg.iface = this->iface.base.type;
        event.msg.type = CIOT_MSG_TYPE_UNKOWN;
        event.msg.data.mqtt.msg.topic = (char *)mm->topic.ptr;
        event.msg.data.mqtt.msg.data = (void *)mm->data.ptr;
        event.msg.data.mqtt.msg.size = mm->data.len;
    }
    this->iface.event_handler(this, &event, this->iface.event_args);
}

static void ciot_mqttc_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    ciot_mqttc_t this = fn_data;

    switch (ev)
    {
    case MG_EV_ERROR:
        printf("MG_EV_ERROR:%d:%s\n", c->id, (char *)ev_data);
        this->status.state = CIOT_MQTT_STATE_ERROR;
        break;
    case MG_EV_MQTT_OPEN:
        printf("MG_EV_MQTT_OPEN\n");
        this->status.conn_count++;
        this->status.state = CIOT_MQTT_STATE_CONNECTED;
        if (this->iface.event_handler != NULL)
        {
            ciot_iface_event_t event = {0};
            event.id = CIOT_IFACE_EVENT_RESPONSE;
            event.size = CIOT_MSG_GET_SIZE(this->status);
            event.msg.id = this->iface.id;
            event.msg.type = CIOT_MSG_TYPE_START;
            event.msg.data.mqtt.status = this->status;
            this->iface.event_handler(this, &event, this->iface.event_args);
        }
        break;
    case MG_EV_MQTT_MSG:
        printf("MG_EV_MQTT_MSG\n");
        if(this->iface.event_handler != NULL)
        {
            struct mg_mqtt_message *hm = (struct mg_mqtt_message *)ev_data;
            ciot_mqttc_on_msg(this, c, hm);
        }
        else
        {
            mg_error(c, "Event Handler is NULL");
        }
        break;
    case MG_EV_CLOSE:
        printf("MG_EV_CLOSE\n");
        this->status.state = CIOT_MQTT_STATE_DISCONNECTED;
        this->connection = NULL;
        if (this->iface.event_handler != NULL)
        {
            ciot_iface_event_t event = {0};
            event.id = CIOT_IFACE_EVENT_RESPONSE;
            event.size = CIOT_MSG_GET_SIZE(this->status);
            event.msg.id = this->iface.id;
            event.msg.type = CIOT_MSG_TYPE_STOP;
            event.msg.data.mqtt.status = this->status;
            this->iface.event_handler(this, &event, this->iface.event_args);
        }
        break;
    default:
        break;
    }
    (void)fn_data;
}