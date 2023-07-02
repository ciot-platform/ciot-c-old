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

typedef struct ciot_mqtt_config
{
    char host[64];
    int port;
    char username[48];
    char password[48];
    uint8_t qos;
    ciot_mqtt_transport_t transport;
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

ciot_err_t ciot_mqtt_set_config(ciot_mqtt_config_t *conf);
ciot_err_t ciot_mqtt_get_config(ciot_mqtt_config_t *config);
ciot_err_t ciot_mqtt_get_status(ciot_mqtt_status_t *status);
ciot_err_t ciot_mqtt_get_info(ciot_mqtt_info_t *info);

ciot_err_t ciot_mqtt_publish(const char *topic, const char *data, int len, int qos, bool retain);
ciot_err_t ciot_mqtt_subscribe(const char *topic, int qos);
ciot_err_t ciot_mqtt_on_data(ciot_mqtt_on_data_cb_t on_data_cb);
ciot_err_t ciot_mqtt_on_connection(ciot_mqtt_on_connection_cb_t on_connection_cb);

ciot_err_t ciot_mqtt_config_from_json(CJSON_PARAMETERS(ciot_mqtt_config_t));
ciot_err_t ciot_mqtt_config_to_json(CJSON_PARAMETERS(ciot_mqtt_config_t));
ciot_err_t ciot_mqtt_info_from_json(CJSON_PARAMETERS(ciot_mqtt_info_t));
ciot_err_t ciot_mqtt_info_to_json(CJSON_PARAMETERS(ciot_mqtt_info_t));
ciot_err_t ciot_mqtt_status_from_json(CJSON_PARAMETERS(ciot_mqtt_status_t));
ciot_err_t ciot_mqtt_status_to_json(CJSON_PARAMETERS(ciot_mqtt_status_t));

#endif  //!__CIOT_MQTT__H__