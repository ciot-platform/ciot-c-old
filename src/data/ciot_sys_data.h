/**
 * @file ciot_sys_data.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_SYS_DATA__H__
#define __CIOT_SYS_DATA__H__

#include <inttypes.h>
#include "ciot_config.h"

typedef enum ciot_sys_req_id
{
    CIOT_SYS_REQ_UNKNONW,
    CIOT_SYS_REQ_RESTART,
    CIOT_SYS_REQ_FACTORY_RESET,
    CIOT_SYS_REQ_SAVE,
} ciot_sys_req_id_t;

typedef struct __attribute__((packed))
{
    int rst_reason;
    int rst_count;
    uint32_t free_memory;
    uint32_t lifetime;
} ciot_sys_status_t;

typedef struct __attribute__((packed))
{

} ciot_sys_cfg_t;

typedef union ciot_sys_req_data
{

} ciot_sys_req_data_u;

typedef struct __attribute__((packed))
{
    ciot_sys_req_id_t id;
    ciot_sys_req_data_u data;
} ciot_sys_req_t;

typedef union ciot_sys_data
{
    ciot_sys_cfg_t config;
    ciot_sys_status_t status;
    ciot_sys_req_t request;
} ciot_sys_data_u;

#endif  //!__CIOT_SYS_DATA__H__