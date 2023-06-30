/**
 * @file ciot_ntp.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_NTP__H__
#define __CIOT_NTP__H__

#include <inttypes.h>
#include <stdbool.h>
#include <time.h>

#include "ciot_err.h"
#include "ciot_json.h"

typedef enum ciot_ntp_sync_state
{
    CIOT_NTP_SYNC_STATE_RESET,
    CIOT_NTP_SYNC_STATE_COMPLETED,
    CIOT_NTP_SYNC_STATE_IN_PROGRESS,
} ciot_ntp_sync_state_t;

typedef struct ciot_ntp_config
{
    uint8_t op_mode;
    uint8_t sync_mode;
    uint32_t sync_interval;
    char timezone[32];
    char server1[48];
    char server2[48];
    char server3[48];
    int timeout;
} ciot_ntp_config_t;

typedef struct ciot_ntp_info
{
    bool available;
} ciot_ntp_info_t;

typedef struct ciot_ntp_status
{
    bool init;
    bool sync;
    ciot_ntp_sync_state_t state;
    int sync_count;
    time_t last_sync;
} ciot_ntp_status_t;

ciot_err_t ciot_ntp_set_config(ciot_ntp_config_t *conf);
ciot_err_t ciot_ntp_get_config(ciot_ntp_config_t *config);
ciot_err_t ciot_ntp_get_status(ciot_ntp_status_t *status);
ciot_err_t ciot_ntp_get_info(ciot_ntp_info_t *info);

ciot_err_t ciot_ntp_config_from_json(CJSON_PARAMETERS(ciot_ntp_config_t));
ciot_err_t ciot_ntp_config_to_json(CJSON_PARAMETERS(ciot_ntp_config_t));
ciot_err_t ciot_ntp_info_from_json(CJSON_PARAMETERS(ciot_ntp_info_t));
ciot_err_t ciot_ntp_info_to_json(CJSON_PARAMETERS(ciot_ntp_info_t));
ciot_err_t ciot_ntp_status_from_json(CJSON_PARAMETERS(ciot_ntp_status_t));
ciot_err_t ciot_ntp_status_to_json(CJSON_PARAMETERS(ciot_ntp_status_t));

#endif  //!__CIOT_NTP__H__