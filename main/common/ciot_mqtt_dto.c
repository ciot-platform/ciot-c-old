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

static ciot_err_t ciot_mqtt_config_data_union_from_json(CJSON_PARAMETERS(ciot_mqtt_config_data_u), ciot_mqtt_config_type_t config_type);
static ciot_err_t ciot_mqtt_config_data_union_to_json(CJSON_PARAMETERS(ciot_mqtt_config_data_u), ciot_mqtt_config_type_t config_type);

static ciot_err_t ciot_mqtt_config_data_from_json(CJSON_PARAMETERS(ciot_mqtt_config_data_t));
static ciot_err_t ciot_mqtt_config_data_to_json(CJSON_PARAMETERS(ciot_mqtt_config_data_t));

static ciot_err_t ciot_mqtt_config_connection_from_json(CJSON_PARAMETERS(ciot_mqtt_config_connection_t));
static ciot_err_t ciot_mqtt_config_connection_to_json(CJSON_PARAMETERS(ciot_mqtt_config_connection_t));

static ciot_err_t ciot_mqtt_config_topics_from_json(CJSON_PARAMETERS(ciot_mqtt_config_topics_t));
static ciot_err_t ciot_mqtt_config_topics_to_json(CJSON_PARAMETERS(ciot_mqtt_config_topics_t));

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
