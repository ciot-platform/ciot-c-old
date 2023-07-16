/**
 * @file ciot_ota.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_OTA__H__
#define __CIOT_OTA__H__

#include "ciot_err.h"
#include "ciot_json.h"

#define BUFFSIZE 1024

typedef enum ciot_ota_state
{
    CIOT_OTA_STATE_ERROR=-1,
    CIOT_OTA_STATE_IDLE,
    CIOT_OTA_STATE_IN_PROGRESS,
    CIOT_OTA_STATE_DONE,
} ciot_ota_state_t;

typedef struct ciot_ota_callbacks
{
    void (*on_ota_started)(void);
    void (*on_ota_completed)(void);
    void (*on_ota_error)(ciot_err_t);
} ciot_ota_callbacks_t;

typedef struct ciot_ota_config
{
    char url[256];
    bool force;
    bool encrypted;
    int timeout;
} ciot_ota_config_t;

typedef struct ciot_ota_info
{
    uint8_t sha256[32];
} ciot_ota_info_t;

typedef struct ciot_ota_status
{
    ciot_ota_state_t state;
} ciot_ota_status_t;

typedef struct ciot_ota
{
    ciot_ota_callbacks_t callbacks;
    ciot_ota_config_t config;
    ciot_err_t err_code;
    ciot_ota_status_t status;
    ciot_ota_info_t info;
    char buffer[BUFFSIZE];
    void *task;
} ciot_ota_t;

ciot_err_t ciot_ota_set_config(ciot_ota_t *ota, ciot_ota_config_t *conf);
ciot_err_t ciot_ota_get_config(ciot_ota_t *ota, ciot_ota_config_t *conf);
ciot_err_t ciot_ota_get_status(ciot_ota_t *ota, ciot_ota_status_t *status);
ciot_err_t ciot_ota_get_info(ciot_ota_t *ota, ciot_ota_info_t *info);
ciot_err_t ciot_ota_set_callbacks(ciot_ota_t *ota, ciot_ota_callbacks_t *callbacks);

ciot_err_t ciot_ota_start(void);

ciot_err_t ciot_ota_config_from_json(CJSON_PARAMETERS(ciot_ota_config_t));
ciot_err_t ciot_ota_config_to_json(CJSON_PARAMETERS(ciot_ota_config_t));
ciot_err_t ciot_ota_info_from_json(CJSON_PARAMETERS(ciot_ota_info_t));
ciot_err_t ciot_ota_info_to_json(CJSON_PARAMETERS(ciot_ota_info_t));
ciot_err_t ciot_ota_status_from_json(CJSON_PARAMETERS(ciot_ota_status_t));
ciot_err_t ciot_ota_status_to_json(CJSON_PARAMETERS(ciot_ota_status_t));

#endif  //!__CIOT_OTA__H__