/**
 * @file ciot_data_model.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_DATA_MODEL__H__
#define __CIOT_DATA_MODEL__H__

typedef enum ciot_data_model_state
{
    CIOT_DATA_MODEL_STATE_IDLE,
} ciot_data_model_state_t;

typedef enum ciot_data_model_req_id
{
    CIOT_DATA_MODEL_REQ_UNKNOWN,
} ciot_data_model_req_id_t;

typedef struct __attribute__((packed))
{

} ciot_data_model_cfg_t;

typedef struct __attribute__((packed))
{
    ciot_data_model_state_t state;
} ciot_data_model_status_t;

typedef union ciot_data_model_req_data
{

} ciot_data_model_req_data_u;

typedef struct __attribute__((packed))
{
    ciot_data_model_req_id_t id;
    ciot_data_model_req_data_u data;
} ciot_data_model_req_t;

typedef union ciot_data_model_data
{
    #if CIOT_CONFIG_FEATURE_DATA_MODEL
    ciot_data_model_cfg_t config;
    ciot_data_model_status_t status;
    ciot_data_model_req_t request;
    #endif
} ciot_data_model_data_u;

#endif //!__CIOT_DATA_MODEL__H__