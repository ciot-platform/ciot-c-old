/**
 * @file ciot_mqtt_dto.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-07-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_mqtt.h"
#include "ciot_data_io.h"
#include "ciot_msg_dto.h"
#include "ciot_app.h"

static ciot_err_t ciot_mqtt_config_data_union_from_json(CJSON_PARAMETERS(ciot_mqtt_config_data_u), ciot_mqtt_config_type_t config_type);
static ciot_err_t ciot_mqtt_config_data_union_to_json(CJSON_PARAMETERS(ciot_mqtt_config_data_u), ciot_mqtt_config_type_t config_type);

static ciot_err_t ciot_mqtt_config_data_from_json(CJSON_PARAMETERS(ciot_mqtt_config_data_t));
static ciot_err_t ciot_mqtt_config_data_to_json(CJSON_PARAMETERS(ciot_mqtt_config_data_t));

static ciot_err_t ciot_mqtt_config_connection_from_json(CJSON_PARAMETERS(ciot_mqtt_config_connection_t));
static ciot_err_t ciot_mqtt_config_connection_to_json(CJSON_PARAMETERS(ciot_mqtt_config_connection_t));

static ciot_err_t ciot_mqtt_config_topics_from_json(CJSON_PARAMETERS(ciot_mqtt_config_topics_t));
static ciot_err_t ciot_mqtt_config_topics_to_json(CJSON_PARAMETERS(ciot_mqtt_config_topics_t));

static ciot_err_t ciot_mqtt_set_config_connection(ciot_mqtt_t *mqtt, ciot_mqtt_config_connection_t *config);
static ciot_err_t ciot_mqtt_set_config_topics(ciot_mqtt_t *mqtt, ciot_mqtt_config_topics_t *config);

ciot_err_t ciot_mqtt_set_config(ciot_mqtt_t *mqtt, ciot_mqtt_config_t *conf)
{
    switch (conf->config_type)
    {
    case CIOT_MQTT_CONFIG_CONNECTION:
        return ciot_mqtt_set_config_connection(mqtt, &conf->data.connection);
    case CIOT_MQTT_CONFIG_TOPICS:
        return ciot_mqtt_set_config_topics(mqtt, &conf->data.topics);
    case CIOT_MQTT_CONFIG_ALL:
        CIOT_ERROR_RETURN(ciot_mqtt_set_config_connection(mqtt, &conf->data.config.connection));
        return ciot_mqtt_set_config_topics(mqtt, &conf->data.config.topics);
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

ciot_err_t ciot_mqtt_get_config(ciot_mqtt_t *mqtt, ciot_mqtt_config_t *config)
{
    config->config_type = CIOT_MQTT_CONFIG_ALL;
    config->data.config = mqtt->config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_get_status(ciot_mqtt_t *mqtt, ciot_mqtt_status_t *status)
{
    *status = mqtt->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_get_info(ciot_mqtt_t *mqtt, ciot_mqtt_info_t *info)
{
    *info = mqtt->info;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_on_data(ciot_mqtt_t *mqtt, ciot_mqtt_on_data_cb_t on_data_cb)
{
    mqtt->on_data_cb = on_data_cb;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_on_connection(ciot_mqtt_t *mqtt, ciot_mqtt_on_connection_cb_t on_connection_cb)
{
    mqtt->on_connection_cb = on_connection_cb;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_handle_data(ciot_mqtt_t *mqtt, void *data)
{
    ciot_msg_t msg = {0};
    ciot_msg_data_t *msg_data = (ciot_msg_data_t*)data;
    ciot_err_t err = ciot_data_deserialize_msg(msg_data, &msg);
    if (err != CIOT_ERR_OK)
    {
        char err_msg[28];
        sprintf(err_msg, CIOT_ERROR_MASK, err);
        ciot_mqtt_publish(mqtt, mqtt->config.topics.response, err_msg, strlen(err_msg), mqtt->config.connection.qos, false);
    }
    else
    {
        err = ciot_app_send_msg(&msg);
        if (err == CIOT_ERR_OK)
        {
            ciot_data_t resp_data = {.data_type = mqtt->config.topics.data_type};
            ciot_msg_response_t resp = {0};
            ciot_app_wait_process(20000);
            ciot_app_get_msg_response(&resp);

            err = ciot_data_serialize_resp(&resp, &resp_data);
            if (err != CIOT_ERR_OK)
            {
                char err_msg[28];
                sprintf(err_msg, CIOT_ERROR_MASK, err);
                ciot_mqtt_publish(mqtt, mqtt->config.topics.response, err_msg, strlen(err_msg), mqtt->config.connection.qos, false);
            }
            else
            {
                ciot_mqtt_publish(mqtt, mqtt->config.topics.response, resp_data.buffer.content, resp_data.buffer.size, mqtt->config.connection.qos, false);
                free(resp_data.buffer.content);
            }
        }
    }
    return err;
}

ciot_err_t ciot_mqtt_config_from_json(CJSON_PARAMETERS(ciot_mqtt_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(config_type);
    CJSON_GET_UNION(data, ciot_mqtt_config_data_union_from_json, config_type);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_config_to_json(CJSON_PARAMETERS(ciot_mqtt_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(config_type);
    CJSON_ADD_UNION(data, ciot_mqtt_config_data_union_to_json, config_type);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_info_from_json(CJSON_PARAMETERS(ciot_mqtt_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(version);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_info_to_json(CJSON_PARAMETERS(ciot_mqtt_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(version);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_status_from_json(CJSON_PARAMETERS(ciot_mqtt_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(state);
    CJSON_GET_NUMBER(connection);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_mqtt_status_to_json(CJSON_PARAMETERS(ciot_mqtt_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(state);
    CJSON_ADD_NUMBER(connection);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_data_union_from_json(CJSON_PARAMETERS(ciot_mqtt_config_data_u), ciot_mqtt_config_type_t config_type)
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_OBJ_UNION_CHILD(connection, ciot_mqtt_config_connection_from_json, config_type, CIOT_MQTT_CONFIG_CONNECTION);
    CJSON_GET_OBJ_UNION_CHILD(topics, ciot_mqtt_config_topics_from_json, config_type, CIOT_MQTT_CONFIG_TOPICS);
    CJSON_GET_OBJ_UNION_CHILD(config, ciot_mqtt_config_data_from_json, config_type, CIOT_MQTT_CONFIG_ALL);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_data_union_to_json(CJSON_PARAMETERS(ciot_mqtt_config_data_u), ciot_mqtt_config_type_t config_type)
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ_UNION_CHILD(connection, ciot_mqtt_config_connection_to_json, config_type, CIOT_MQTT_CONFIG_CONNECTION);
    CJSON_ADD_OBJ_UNION_CHILD(topics, ciot_mqtt_config_topics_to_json, config_type, CIOT_MQTT_CONFIG_TOPICS);
    if (config_type == CIOT_MQTT_CONFIG_ALL)
    {
        int err = ciot_mqtt_config_data_to_json(json, &ptr->config);
        if (err != CIOT_ERR_OK)
            return err;
    };
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_data_from_json(CJSON_PARAMETERS(ciot_mqtt_config_data_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_OBJ(connection, ciot_mqtt_config_connection_from_json);
    CJSON_GET_OBJ(topics, ciot_mqtt_config_topics_from_json);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_data_to_json(CJSON_PARAMETERS(ciot_mqtt_config_data_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_OBJ(connection, ciot_mqtt_config_connection_to_json);
    CJSON_ADD_OBJ(topics, ciot_mqtt_config_topics_to_json);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_connection_from_json(CJSON_PARAMETERS(ciot_mqtt_config_connection_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_CHAR_ARRAY(host);
    CJSON_GET_NUMBER(port);
    CJSON_GET_CHAR_ARRAY(username);
    CJSON_GET_CHAR_ARRAY(password);
    CJSON_GET_NUMBER(qos);
    CJSON_GET_NUMBER(transport);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_connection_to_json(CJSON_PARAMETERS(ciot_mqtt_config_connection_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_CHAR_ARRAY(host);
    CJSON_ADD_NUMBER(port);
    CJSON_ADD_CHAR_ARRAY(username);
    CJSON_ADD_CHAR_ARRAY(password);
    CJSON_ADD_NUMBER(qos);
    CJSON_ADD_NUMBER(transport);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_topics_from_json(CJSON_PARAMETERS(ciot_mqtt_config_topics_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_CHAR_ARRAY(message);
    CJSON_GET_CHAR_ARRAY(response);
    CJSON_GET_NUMBER(data_type);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_config_topics_to_json(CJSON_PARAMETERS(ciot_mqtt_config_topics_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_CHAR_ARRAY(message);
    CJSON_ADD_CHAR_ARRAY(response);
    CJSON_ADD_NUMBER(data_type);
    return CIOT_ERR_OK;
}

static ciot_err_t ciot_mqtt_set_config_connection(ciot_mqtt_t *mqtt, ciot_mqtt_config_connection_t *conf)
{
    memcpy(&mqtt->config.connection, conf, sizeof(mqtt->config.connection));
    return ciot_mqtt_connect(mqtt);
}

static ciot_err_t ciot_mqtt_set_config_topics(ciot_mqtt_t *mqtt, ciot_mqtt_config_topics_t *conf)
{
    memcpy(&mqtt->config.topics, conf, sizeof(mqtt->config.topics));
    return ciot_mqtt_subscribe(mqtt, mqtt->config.topics.message, mqtt->config.topics.qos);
}