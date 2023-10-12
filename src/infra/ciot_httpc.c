/**
 * @file ciot_httpc.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdlib.h>
#include <string.h>
#include "ciot_httpc.h"
#include "mongoose.h"

struct ciot_httpc
{
    ciot_iface_t iface;
    ciot_httpc_cfg_t cfg;
    ciot_httpc_status_t status;
    struct mg_mgr *mgr;
    struct mg_connection *connection;
    ciot_httpc_req_send_t req;
};

static void ciot_httpc_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

ciot_httpc_t ciot_httpc_new(void *handle)
{
    ciot_httpc_t this = calloc(1, sizeof(struct ciot_httpc));
    this->iface.base.ptr = this;
    this->iface.base.start = (ciot_iface_start_fn *)ciot_httpc_start;
    this->iface.base.stop = (ciot_iface_stop_fn *)ciot_httpc_stop;
    this->iface.base.process_req = (ciot_iface_process_req_fn *)ciot_httpc_process_req;
    this->iface.base.send_data = (ciot_iface_send_data_fn *)ciot_httpc_send_data;
    this->iface.base.cfg.ptr = &this->cfg;
    this->iface.base.cfg.size = sizeof(this->cfg);
    this->iface.base.status.ptr = &this->status;
    this->iface.base.status.size = sizeof(this->status);
    this->iface.base.type = CIOT_IFACE_TYPE_HTTP_CLIENT;
    this->mgr = handle;
    return this;
}

ciot_err_t ciot_httpc_start(ciot_httpc_t this, ciot_httpc_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(cfg);
    memcpy(&this->cfg, cfg, sizeof(this->cfg));
    return CIOT_OK;
}

ciot_err_t ciot_httpc_stop(ciot_httpc_t this)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_httpc_process_req(ciot_httpc_t this, ciot_httpc_req_t *req)
{
    switch (req->id)
    {
    case CIOT_HTTPC_REQ_SEND_DATA:
        memcpy(&this->req, &req->data.send, sizeof(this->req));
        this->status.state = CIOT_HTTPC_STATE_CONNECTING;
        mg_http_connect(this->mgr, this->cfg.url, ciot_httpc_event_handler, this);
        break;
    default:
        break;
    }
}

ciot_err_t ciot_httpc_send_data(ciot_httpc_t this, uint8_t *data, int size)
{
    if (this->status.state == CIOT_HTTPC_STATE_IDLE || this->status.state == CIOT_HTTPC_STATE_TIMEOUT)
    {
        memcpy(&this->req.cfg, &this->cfg, sizeof(this->req.cfg));
        memcpy(this->req.body, data, size);
        this->req.content_length = size;
        this->status.state = CIOT_HTTPC_STATE_CONNECTING;
        mg_http_connect(this->mgr, this->cfg.url, ciot_httpc_event_handler, this);
        return CIOT_OK;
    }
    else
    {
        return CIOT_ERR_BUSY;
    }
}

static void ciot_httpc_on_msg(ciot_httpc_t this, struct mg_connection *c, struct mg_http_message *hm)
{
    ciot_iface_event_t event = {0};
    if (strcmp(this->req.cfg.url, this->cfg.url) == 0)
    {
        event.id = CIOT_IFACE_EVENT_RESPONSE;
        event.size = hm->body.len;
        memcpy(&event.msg, hm->body.ptr, hm->body.len);
    }
    else
    {
        event.id = CIOT_HTTPC_EVENT_DATA;
        event.size = CIOT_MSG_GET_SIZE(event.msg.data.httpc);
        event.msg.id = this->iface.id;
        event.msg.iface = this->iface.base.type;
        event.msg.type = CIOT_MSG_TYPE_UNKOWN;
        event.msg.data.httpc.msg.url = (char *)hm->uri.ptr;
        event.msg.data.httpc.msg.data = (void *)hm->body.ptr;
        event.msg.data.httpc.msg.size = hm->body.len;
    }
    this->status.state = CIOT_HTTPC_STATE_IDLE;
    this->iface.event_handler(this, &event, this->iface.event_args);
}

static void ciot_httpc_event_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    ciot_httpc_t this = fn_data;
    mg_event_t mg_ev = ev;

    switch (mg_ev)
    {
    case MG_EV_OPEN:
        *(uint64_t *)c->data = mg_millis() + this->cfg.timeout;
        break;
    case MG_EV_POLL:
        if (mg_millis() > *(uint64_t *)c->data && (c->is_connecting || c->is_resolving))
        {
            this->status.state = CIOT_HTTPC_STATE_TIMEOUT;
            mg_error(c, "Connect timeout");
        }
        break;
    case MG_EV_CONNECT:
    {
        this->status.state = CIOT_HTTPC_STATE_CONNECTED;
        struct mg_str host = mg_url_host(this->req.cfg.url);
        int content_length = this->req.content_length;
        mg_printf(c,
                  "%s %s HTTP/1.0\r\n"
                  "Host: %.*s\r\n"
                  "Content-Type: octet-stream\r\n"
                  "Content-Length: %d\r\n"
                  "\r\n",
                  this->req.cfg.method, mg_url_uri(this->req.cfg.url), (int)host.len,
                  host.ptr, content_length);
        mg_send(c, this->req.body, content_length);
        break;
    }
    case MG_EV_HTTP_MSG:
        if(this->iface.event_handler != NULL)
        {
            struct mg_http_message *hm = (struct mg_http_message *)ev_data;
            ciot_httpc_on_msg(this, c, hm);
        }
        else
        {
            mg_error(c, "Event Handler is NULL");
        }
    break;
    default:
        break;
    }
}