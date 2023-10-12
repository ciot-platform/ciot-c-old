/**
 * @file ciot_mqttc_data.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_MQTT_DATA__H__
#define __CIOT_MQTT_DATA__H__

#include "ciot_config.h"
#include <inttypes.h>

#define CIOT_MQTT_CLIENT_ID_LEN 32
#define CIOT_MQTT_URL_LEN 64
#define CIOT_MQTT_USER_LEN 32
#define CIOT_MQTT_PASS_LEN 32
#define CIOT_MQTT_TOPIC_LEN 32
#define CIOT_MQTT_MSG_LEN 256

typedef enum __attribute__((packed))
{
    CIOT_MQTT_STATE_ERROR = -1,
    CIOT_MQTT_STATE_DISCONNECTED,
    CIOT_MQTT_STATE_CONNECTING,
    CIOT_MQTT_STATE_CONNECTED,
} ciot_mqttc_state_t;

typedef enum __attribute__((packed))
{
    CIOT_MQTT_TRANSPORT_UNKNOWN,
    CIOT_MQTT_TRANSPORT_OVER_TCP,
    CIOT_MQTT_TRANSPORT_OVER_SSL,
    CIOT_MQTT_TRANSPORT_OVER_WS,
    CIOT_MQTT_TRANSPORT_OVER_WSS,
} ciot_mqttc_transport_t;

typedef enum __attribute__((packed))
{
    CIOT_MQTT_REQ_UNKNONW,
    CIOT_MQTT_REQ_PUBLISH,
    CIOT_MQTT_REQ_SUBSCRIBE,
} ciot_mqttc_req_id_t;

typedef struct __attribute__((packed))
{
    int tls_last_err;
    int tls_stack_err;
    int tls_cert_verify_flags;
    int type;
    int code;
    int transport_sock;
} ciot_mqttc_error_t;

typedef struct __attribute__((packed))
{
    char d2b[CIOT_MQTT_TOPIC_LEN];
    char b2d[CIOT_MQTT_TOPIC_LEN];
} ciot_mqttc_topics_cfg_t;

typedef struct __attribute__((packed))
{
    char client_id[CIOT_MQTT_CLIENT_ID_LEN];
    char url[CIOT_MQTT_URL_LEN];
    uint32_t port;
    char user[CIOT_MQTT_USER_LEN];
    char pass[CIOT_MQTT_PASS_LEN];
    uint8_t qos;
    ciot_mqttc_topics_cfg_t topics;
} ciot_mqttc_cfg_t;

typedef struct __attribute__((packed))
{
    ciot_mqttc_state_t state;
    uint8_t conn_count;
    uint16_t data_rate;
    time_t last_msg_time;
    ciot_mqttc_error_t error;
} ciot_mqttc_status_t;

typedef struct __attribute__((packed))
{
    char topic[CIOT_MQTT_TOPIC_LEN];
    uint8_t msg[CIOT_MQTT_MSG_LEN];
    int size;
    uint8_t qos;
} ciot_mqttc_req_publish_t;

typedef struct __attribute__((packed))
{
    char topic[CIOT_MQTT_TOPIC_LEN];
    uint8_t qos;
} ciot_mqttc_req_subscribe_t;

typedef union ciot_mqttc_req_data
{
    ciot_mqttc_req_publish_t publish;
    ciot_mqttc_req_subscribe_t subscribe;
} ciot_mqttc_req_data_u;

typedef struct __attribute__((packed))
{
    ciot_mqttc_req_id_t id;
    ciot_mqttc_req_data_u data;
} ciot_mqttc_req_t;

typedef struct __attribute__((packed))
{
    char *topic;
    void *data;
    int size;
} ciot_mqttc_event_msg_t;

typedef union ciot_mqttc_data
{
#if CIOT_CONFIG_FEATURE_MQTT
    ciot_mqttc_cfg_t config;
    ciot_mqttc_status_t status;
    ciot_mqttc_req_t request;
    ciot_mqttc_event_msg_t msg;
#endif
} ciot_mqttc_data_u;

#endif //!__CIOT_MQTT_DATA__H__