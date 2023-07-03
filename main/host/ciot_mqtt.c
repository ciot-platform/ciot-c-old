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

typedef struct ciot_mqtt
{
    void *handle;
    ciot_mqtt_config_data_t config;
    ciot_mqtt_info_t info;
    ciot_mqtt_status_t status;
    ciot_mqtt_on_data_cb_t *on_data_cb;
    ciot_mqtt_on_connection_cb_t *on_connection_cb;
} ciot_mqtt_t;

static ciot_mqtt_t this;

static void ciot_mqtt_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data);
static ciot_err_t ciot_mqtt_set_config_connection(ciot_mqtt_config_connection_t *config);
static ciot_err_t ciot_mqtt_set_config_topics(ciot_mqtt_config_topics_t *config);
static void ciot_mqtt_wait_connection(int timeout);

ciot_err_t ciot_mqtt_set_config(ciot_mqtt_config_t *conf)
{
    switch (conf->config_type)
    {
    case CIOT_MQTT_CONFIG_CONNECTION:
        return ciot_mqtt_set_config_connection(&conf->data.connection);
    case CIOT_MQTT_CONFIG_TOPICS:
        return ciot_mqtt_set_config_topics(&conf->data.topics);
    case CIOT_MQTT_CONFIG_ALL:
        CIOT_ERROR_RETURN(ciot_mqtt_set_config_connection(&conf->data.config.connection));
        return ciot_mqtt_set_config_topics(&conf->data.config.topics);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

ciot_err_t ciot_mqtt_get_config(ciot_mqtt_config_t *config)
{
    config->config_type = CIOT_MQTT_CONFIG_ALL;
    config->data.config = this.config;
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
        .retain = retain};
    mg_mqtt_pub(this.handle, &opts);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_subscribe(const char *topic, int qos)
{
    struct mg_mqtt_opts opts = {
        .topic = mg_str(topic),
        .qos = qos};
    mg_mqtt_sub(this.handle, &opts);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_on_data(ciot_mqtt_on_data_cb_t on_data_cb)
{
    this.on_data_cb = on_data_cb;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_on_connection(ciot_mqtt_on_connection_cb_t on_connection_cb)
{
    this.on_connection_cb = on_connection_cb;
    return CIOT_ERR_OK;
}

static void ciot_mqtt_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    ciot_err_t err = CIOT_ERR_OK;
    switch (ev)
    {
    case MG_EV_ERROR:
        printf("MG_EV_ERROR:%d:%s\n", c->id, (char *)ev_data);
        this.status.state = CIOT_MQTT_STATE_ERROR;
        break;
    case MG_EV_MQTT_OPEN:
        printf("MG_EV_MQTT_OPEN\n");
        this.status.connection++;
        this.status.state = CIOT_MQTT_STATE_CONNECTED;
        if (this.on_connection_cb != NULL)
        {
            this.on_connection_cb();
        }
        break;
    case MG_EV_MQTT_MSG:
        printf("MG_EV_MQTT_MSG\n");
        struct mg_mqtt_message *mm = (struct mg_mqtt_message *)ev_data;
        if (strncmp(mm->topic.ptr, this.config.topics.message, mm->topic.len) == 0)
        {
            ciot_msg_t msg = {0};
            ciot_data_t msg_data = {
                .data_type = this.config.topics.data_type,
                .buffer.content = (void *)mm->data.ptr,
                .buffer.size = mm->data.len,
            };
            err = ciot_data_deserialize_msg(&msg_data, &msg);
            if (err != CIOT_ERR_OK)
            {
                char err_msg[28];
                sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK, err);
                ciot_mqtt_publish(this.config.topics.response, err_msg, strlen(err_msg), this.config.connection.qos, false);
            }
            else
            {
                err = ciot_app_send_msg(&msg);
                if (err == CIOT_ERR_OK)
                {
                    ciot_data_t resp_data = {.data_type = this.config.topics.data_type};
                    ciot_msg_response_t resp = {0};
                    ciot_app_wait_process(20000);
                    ciot_app_get_msg_response(&resp);

                    err = ciot_data_serialize_resp(&resp, &resp_data);
                    if (err != CIOT_ERR_OK)
                    {
                        char err_msg[28];
                        sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK, err);
                        ciot_mqtt_publish(this.config.topics.response, err_msg, strlen(err_msg), this.config.connection.qos, false);
                    }
                    else
                    {
                        ciot_mqtt_publish(this.config.topics.response, resp_data.buffer.content, resp_data.buffer.size, this.config.connection.qos, false);
                        free(resp_data.buffer.content);
                    }
                }
            }
        }
        if (this.on_data_cb != NULL)
        {
            this.on_data_cb(mm->topic.ptr, mm->topic.len, mm->data.ptr, mm->data.len);
        }
        break;
    case MG_EV_CLOSE:
        printf("MG_EV_CLOSE\n");
        this.status.state = CIOT_MQTT_STATE_DISCONNECTED;
        this.handle = NULL;
        break;
    default:
        break;
    }
    (void)fn_data;
}

static ciot_err_t ciot_mqtt_set_config_connection(ciot_mqtt_config_connection_t *conf)
{
    memcpy(&this.config.connection, conf, sizeof(this.config.connection));
    struct mg_mqtt_opts opts = {
        .clean = true,
        .qos = this.config.connection.qos,
        .version = 4,
        .user = mg_str(this.config.connection.username),
        .pass = mg_str(this.config.connection.password)};
    this.status.state = CIOT_MQTT_STATE_CONNECTING;
    if (this.handle == NULL)
        this.handle = mg_mqtt_connect(&mgr, this.config.connection.host, &opts, ciot_mqtt_event_handler, NULL);
    ciot_mqtt_wait_connection(CIOT_MQTT_TIMEOUT);
    return this.status.state == CIOT_MQTT_STATE_CONNECTED ? CIOT_ERR_OK : CIOT_ERR_FAIL;
}

static ciot_err_t ciot_mqtt_set_config_topics(ciot_mqtt_config_topics_t *conf)
{
    memcpy(&this.config.topics, conf, sizeof(this.config.topics));
    return ciot_mqtt_subscribe(this.config.topics.message, this.config.topics.qos);
}

static void ciot_mqtt_wait_connection(int timeout)
{
    time_t start_time = time(0);
    while (this.status.state == CIOT_MQTT_STATE_CONNECTING)
    {
        time_t current_time = time(0);
        if((start_time + timeout) < current_time)
        {
            return;
        }
    }
}