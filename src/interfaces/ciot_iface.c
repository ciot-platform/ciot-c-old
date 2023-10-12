/**
 * @file ciot_iface.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string.h>
#include "ciot_err.h"
#include "ciot_iface.h"

ciot_err_t ciot_iface_start(ciot_iface_t *this, ciot_msg_data_u *cfg)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(cfg);
    CIOT_ERR_NULL_CHECK(this->base.ptr);;
    CIOT_ERR_NULL_CHECK(this->base.start);
    return this->base.start(this->base.ptr, cfg);
}

ciot_err_t ciot_iface_stop(ciot_iface_t *this)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(this->base.ptr);
    CIOT_ERR_NULL_CHECK(this->base.stop);
    return this->base.stop(this->base.ptr);
}

ciot_err_t ciot_iface_get_cfg(ciot_iface_t *this, ciot_msg_data_u *cfg)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(cfg);
    CIOT_ERR_NULL_CHECK(this->base.ptr);
    memcpy(cfg, this->base.cfg.ptr, this->base.cfg.size);
    return CIOT_OK;
}

ciot_err_t ciot_iface_get_status(ciot_iface_t *this, ciot_msg_data_u *status)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(status);
    CIOT_ERR_NULL_CHECK(this->base.ptr);
    memcpy(status, this->base.status.ptr, this->base.status.size);
    return CIOT_OK;
}

ciot_err_t ciot_iface_process_req(ciot_iface_t *this, ciot_msg_data_u *req)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(req);
    CIOT_ERR_NULL_CHECK(this->base.ptr);
    CIOT_ERR_NULL_CHECK(this->base.process_req);
    return this->base.process_req(this->base.ptr, req);
}

ciot_err_t ciot_iface_send_data(ciot_iface_t *this, void *data, int size)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(data);
    CIOT_ERR_NULL_CHECK(this->base.ptr);
    CIOT_ERR_NULL_CHECK(this->base.send_data);
    return this->base.send_data(this->base.ptr, data, size);
}

ciot_err_t ciot_iface_process_msg(ciot_iface_t *this, ciot_msg_t *msg, int *size)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(msg);

    if(msg->id != this->id) {
        return CIOT_ERR_INVALID_ID;
    }

    switch (msg->type)
    {
    case CIOT_MSG_TYPE_START:
        return ciot_iface_start(this, &msg->data);
    case CIOT_MSG_TYPE_STOP:
        return ciot_iface_stop(this);
    case CIOT_MSG_TYPE_GET_CONFIG:
        *size = CIOT_MSG_GET_SIZE(this->base.cfg.size);
        return ciot_iface_get_cfg(this, &msg->data);
    case CIOT_MSG_TYPE_GET_STATUS:
        *size = CIOT_MSG_GET_SIZE(this->base.status.size);
        return ciot_iface_get_status(this, &msg->data);
    }

    return CIOT_ERR_INVALID_TYPE;
}

ciot_err_t ciot_iface_register_event(ciot_iface_t *this, ciot_iface_event_handler_t event_handler, void *event_args)
{
    CIOT_ERR_NULL_CHECK(this);
    this->event_handler = event_handler;
    this->event_args = event_args;
    return CIOT_OK;
}
