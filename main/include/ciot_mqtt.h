/**
 * @file ciot_mqtt.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_MQTT__H__
#define __CIOT_MQTT__H__

#include <inttypes.h>

#include "ciot_err.h"
#include "ciot_json.h"

typedef enum
{
    CIOT_MQTT_TRANSPORT_UNKNOWN,
    CIOT_MQTT_TRANSPORT_TCP,
    CIOT_MQTT_TRANSPORT_SSL,
    CIOT_MQTT_TRANSPORT_WS,
    CIOT_MQTT_TRANSPORT_WSS
} ciot_mqtt_transport_t;

typedef enum
{
    CIOT_MQTT_PROTOCOL_UNKNOWN,
    CIOT_MQTT_PROTOCOL_V_3_1,
    CIOT_MQTT_PROTOCOL_V_3_1_1,
    CIOT_MQTT_PROTOCOL_V_5,
} ciot_mqtt_protocol_version_t;

typedef enum
{
    CIOT_MQTT_STATE_ERROR=-1,
    CIOT_MQTT_STATE_DISCONNECTED,
    CIOT_MQTT_STATE_CONNECTING,
    CIOT_MQTT_STATE_CONNECTED,
} ciot_mqtt_state_t;

typedef enum
{
    CIOT_MQTT_CONFIG_UNKNOWN,
    CIOT_MQTT_CONFIG_CONNECTION,
    CIOT_MQTT_CONFIG_TOPICS,
    CIOT_MQTT_CONFIG_ALL
} ciot_mqtt_config_type_t;

typedef struct ciot_mqtt_config_connection
{
    char host[64];
    int port;
    char username[48];
    char password[48];
    uint8_t qos;
    ciot_mqtt_transport_t transport;
} ciot_mqtt_config_connection_t;

typedef struct ciot_mqtt_config_topics
{
    int qos;
    char message[64];
    char response[64];
    uint8_t data_type;
} ciot_mqtt_config_topics_t;

typedef struct ciot_mqtt_config_data_type
{
    ciot_mqtt_config_connection_t connection;
    ciot_mqtt_config_topics_t topics;
} ciot_mqtt_config_data_t;

typedef union ciot_mqtt_config_data_union
{
    ciot_mqtt_config_connection_t connection;
    ciot_mqtt_config_topics_t topics;
    ciot_mqtt_config_data_t config;
} ciot_mqtt_config_data_u;

typedef struct ciot_mqtt_config
{
    ciot_mqtt_config_type_t config_type;
    ciot_mqtt_config_data_u data;
} ciot_mqtt_config_t;

typedef struct ciot_mqtt_info
{
    ciot_mqtt_protocol_version_t version;
} ciot_mqtt_info_t;

typedef struct ciot_mqtt_status
{
    ciot_mqtt_state_t state;
    uint8_t connection;
} ciot_mqtt_status_t;

typedef ciot_err_t (ciot_mqtt_on_data_cb_t)(const char *topic, size_t topic_len, const char *data, size_t data_len);
typedef ciot_err_t (ciot_mqtt_on_connection_cb_t)();

typedef struct ciot_mqtt
{
    void *handle;
    ciot_mqtt_config_data_t config;
    ciot_mqtt_info_t info;
    ciot_mqtt_status_t status;
    ciot_mqtt_on_data_cb_t *on_data_cb;
    ciot_mqtt_on_connection_cb_t *on_connection_cb;
} ciot_mqtt_t;

ciot_err_t ciot_mqtt_set_config(ciot_mqtt_t *mqtt, ciot_mqtt_config_t *conf);
ciot_err_t ciot_mqtt_get_config(ciot_mqtt_t *mqtt, ciot_mqtt_config_t *config);
ciot_err_t ciot_mqtt_get_status(ciot_mqtt_t *mqtt, ciot_mqtt_status_t *status);
ciot_err_t ciot_mqtt_get_info(ciot_mqtt_t *mqtt, ciot_mqtt_info_t *info);
ciot_err_t ciot_mqtt_on_data(ciot_mqtt_t *mqtt, ciot_mqtt_on_data_cb_t on_data_cb);
ciot_err_t ciot_mqtt_handle_data(ciot_mqtt_t *mqtt, void *data);
ciot_err_t ciot_mqtt_on_connection(ciot_mqtt_t *mqtt, ciot_mqtt_on_connection_cb_t on_connection_cb);

ciot_err_t ciot_mqtt_connect(ciot_mqtt_t *mqtt);
ciot_err_t ciot_mqtt_publish(ciot_mqtt_t *mqtt, const char *topic, const char *data, int len, int qos, bool retain);
ciot_err_t ciot_mqtt_subscribe(ciot_mqtt_t *mqtt, const char *topic, int qos);

ciot_err_t ciot_mqtt_config_from_json(CJSON_PARAMETERS(ciot_mqtt_config_t));
ciot_err_t ciot_mqtt_config_to_json(CJSON_PARAMETERS(ciot_mqtt_config_t));
ciot_err_t ciot_mqtt_info_from_json(CJSON_PARAMETERS(ciot_mqtt_info_t));
ciot_err_t ciot_mqtt_info_to_json(CJSON_PARAMETERS(ciot_mqtt_info_t));
ciot_err_t ciot_mqtt_status_from_json(CJSON_PARAMETERS(ciot_mqtt_status_t));
ciot_err_t ciot_mqtt_status_to_json(CJSON_PARAMETERS(ciot_mqtt_status_t));

#endif  //!__CIOT_MQTT__H__