/**
 * @file ciot_n.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdlib.h>
#include <stdbool.h>
#include "ciot.h"

typedef struct ciot_iface_list
{
    ciot_iface_t **items;
    int count;
} ciot_iface_list_t;

struct ciot_n
{
    ciot_iface_list_t iface_list;
    ciot_iface_t *iface_rsp;
    ciot_iface_t *iface_busy;
    ciot_iface_event_handler_t *event_handler;
    void *event_args;
    ciot_state_t state;
};

static ciot_err_t ciot_iface_event_handler(void *sender, ciot_iface_event_t *event, void *event_args);

ciot_t ciot_new(void)
{
    return calloc(1, sizeof(struct ciot_n));
}

ciot_err_t ciot_set_iface_list(ciot_t this, ciot_iface_t *iface_list[], int count)
{
    this->iface_list.items = iface_list;
    this->iface_list.count = count;

    for (size_t i = 0; i < count; i++)
    {
        this->iface_list.items[i]->id = i;
        ciot_iface_register_event(this->iface_list.items[i], ciot_iface_event_handler, this);
    }

    return CIOT_OK;
}

static ciot_err_t ciot_iface_event_handler(void *sender, ciot_iface_event_t *event, void *event_args)
{
    ciot_t this = event_args;
    ciot_iface_t *iface_snd = sender;
    ciot_iface_t *iface_sel = this->iface_list.items[event->msg.id];
    bool sync_msg = event->msg.type == CIOT_MSG_TYPE_GET_CONFIG || event->msg.type == CIOT_MSG_TYPE_GET_STATUS;

    if (this->state == CIOT_STATE_BUSY)
    {
        event->msg.error = CIOT_ERR_BUSY;
        event->msg.iface = this->iface_busy->base.type;
        event->size = CIOT_MSG_SIZE;
        return ciot_iface_send_data(iface_snd, &event->msg, event->size);
    }

    if (event->msg.id >= this->iface_list.count || event->msg.id < 0)
    {
        event->msg.error = CIOT_ERR_INVALID_ID;
        event->size = CIOT_MSG_SIZE;
        return ciot_iface_send_data(this->iface_rsp, &event->msg, event->size);
    }

    switch (event->id)
    {
    case CIOT_IFACE_EVENT_REQUEST:
        if (sync_msg)
        {
            ciot_iface_process_msg(iface_sel, &event->msg, &event->size);
            event->msg.iface = iface_sel->base.type;
            return ciot_iface_send_data(iface_snd, &event->msg, event->size);
        }
        else
        {
            this->iface_rsp = iface_snd;
            this->iface_busy = iface_sel;
            this->state = CIOT_STATE_BUSY;
            return ciot_iface_process_msg(iface_sel, &event->msg, &event->size);
        }
        break;
    case CIOT_IFACE_EVENT_RESPONSE:
        if (this->state == CIOT_STATE_BUSY)
        {
            this->state = CIOT_STATE_IDLE;
            event->msg.iface = iface_sel->base.type;
            return ciot_iface_send_data(this->iface_rsp, &event->msg, event->size);
        }
        else
        {
            return CIOT_ERR_INVALID_ID;
        }
    case CIOT_IFACE_EVENT_CUSTOM:
        // return custom event to main application
        return CIOT_OK;
    default:
        event->msg.error = CIOT_ERR_INVALID_TYPE;
        return ciot_iface_send_data(iface_snd, &event->msg, CIOT_MSG_SIZE);
    }
}
