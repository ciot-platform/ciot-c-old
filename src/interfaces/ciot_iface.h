/**
 * @file ciot_iface.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_IFACE__H__
#define __CIOT_IFACE__H__

#include "ciot_err.h"
#include "ciot_msg_data.h"

typedef enum ciot_iface_event_id
{
    CIOT_IFACE_EVENT_UNKNOWN,
    CIOT_IFACE_EVENT_REQUEST,
    CIOT_IFACE_EVENT_RESPONSE,
    CIOT_IFACE_EVENT_CUSTOM,
} ciot_iface_event_id_t;

typedef struct ciot_iface_event
{
    ciot_iface_event_id_t id;
    ciot_msg_t msg;
    int size;
} ciot_iface_event_t;

typedef ciot_err_t (ciot_iface_start_fn)(void *iface, ciot_msg_data_u *cfg);
typedef ciot_err_t (ciot_iface_stop_fn)(void *iface);
typedef ciot_err_t (ciot_iface_process_req_fn)(void *iface, ciot_msg_data_u *req);
typedef ciot_err_t (ciot_iface_send_data_fn)(void *iface, void *data, int size);
typedef ciot_err_t (ciot_iface_event_handler_t)(void *sender, ciot_iface_event_t *event, void *args);

typedef struct ciot_iface_base_data
{
    void *ptr;
    int size;
} ciot_iface_base_data_t;

typedef struct ciot_iface_base
{
    void *ptr;
    ciot_iface_start_fn *start;
    ciot_iface_stop_fn *stop;
    ciot_iface_process_req_fn *process_req;
    ciot_iface_send_data_fn *send_data;
    ciot_iface_base_data_t cfg;
    ciot_iface_base_data_t status;
    ciot_iface_type_t type;
} ciot_iface_base_t;

typedef ciot_err_t (ciot_iface_event_handler_t)(void *sender, ciot_iface_event_t *event, void *event_args);

typedef struct ciot_iface
{
    uint8_t id;
    ciot_iface_event_handler_t *event_handler;
    void *event_args;
    ciot_iface_base_t base;
} ciot_iface_t;

ciot_err_t ciot_iface_start(ciot_iface_t *this, ciot_msg_data_u *cfg);
ciot_err_t ciot_iface_stop(ciot_iface_t *this);
ciot_err_t ciot_iface_get_cfg(ciot_iface_t *this, ciot_msg_data_u *cfg);
ciot_err_t ciot_iface_get_status(ciot_iface_t *this, ciot_msg_data_u *status);
ciot_err_t ciot_iface_process_req(ciot_iface_t *this, ciot_msg_data_u *req);
ciot_err_t ciot_iface_send_data(ciot_iface_t *this, void *data, int size);
ciot_err_t ciot_iface_process_msg(ciot_iface_t *this, ciot_msg_t *msg, int *size);
ciot_err_t ciot_iface_register_event(ciot_iface_t *this, ciot_iface_event_handler_t event_handler, void *event_args);

#endif  //!__CIOT_IFACE__H__