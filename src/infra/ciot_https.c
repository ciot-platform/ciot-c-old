/**
 * @file ciot_https.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-10
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdlib.h>
#include "ciot_https.h"
#include "mongoose.h"

typedef struct ciot_https_msg
{
    void *data;
    int size;
} ciot_https_msg_t;

struct ciot_https
{
    ciot_iface_t iface;
    ciot_https_cfg_t cfg;
    ciot_https_status_t status;
    struct mg_mgr *mgr;
    struct mg_connection *conn_rx;
    struct mg_connection *conn_tx;
};

static void ciot_https_event_handle(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

ciot_https_t ciot_https_new(void *handle)
{
    ciot_https_t this = calloc(1, sizeof(struct ciot_https));
    this->iface.base.ptr = this;
    this->iface.base.start = (ciot_iface_start_fn *)ciot_https_start;
    this->iface.base.stop = (ciot_iface_stop_fn *)ciot_https_stop;
    this->iface.base.process_req = (ciot_iface_process_req_fn *)ciot_https_process_req;
    this->iface.base.send_data = (ciot_iface_send_data_fn *)ciot_https_send_data;
    this->iface.base.cfg.ptr = &this->cfg;
    this->iface.base.cfg.size = sizeof(this->cfg);
    this->iface.base.status.ptr = &this->status;
    this->iface.base.status.size = sizeof(this->status);
    this->iface.base.type = CIOT_IFACE_TYPE_HTTP_SERVER;
    this->mgr = handle;
    return this;
}

ciot_err_t ciot_https_start(ciot_https_t this, ciot_https_cfg_t *cfg)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(cfg);
    memcpy(&this->cfg, cfg, sizeof(this->cfg));
    this->conn_rx = mg_http_listen(this->mgr, cfg->address, ciot_https_event_handle, this);
    if (this->conn_rx == NULL)
    {
        this->status.state = CIOT_HTTPS_STATE_ERROR;
        return CIOT_FAIL;
    }
    else
    {
        this->status.state = CIOT_HTTPS_STATE_STARTED;
        return CIOT_OK;
    }
}

ciot_err_t ciot_https_stop(ciot_https_t this)
{
    CIOT_ERR_NULL_CHECK(this);
    CIOT_ERR_NULL_CHECK(this->conn_rx);
    mg_close_conn(this->conn_rx);
    this->status.state = CIOT_HTTPS_STATE_STOPPED;
    return CIOT_OK;
}

ciot_err_t ciot_https_process_req(ciot_https_t this, ciot_https_req_t *req)
{
    return CIOT_ERR_NOT_SUPPORTED;
}

ciot_err_t ciot_https_send_data(ciot_https_t this, uint8_t *data, int size)
{
    CIOT_ERR_NULL_CHECK(this);
    mg_printf(this->conn_tx, 
              "HTTP/1.0 200 OK\r\n"
              "Content-Type: octet-stream\r\n"
              "Content-Length: %d\r\n\r\n", size);
    mg_send(this->conn_tx, data, size);
    return CIOT_OK;
}

static void ciot_https_on_msg(ciot_https_t this, struct mg_connection *c, struct mg_http_message *hm)
{
    ciot_iface_event_t event = {0};
    bool is_post = strncmp(hm->method.ptr, "POST", hm->method.len) == 0;
    if (mg_http_match_uri(hm, this->cfg.route) && is_post)
    {
        event.id = CIOT_IFACE_EVENT_REQUEST;
        memcpy(&event.msg, hm->body.ptr, hm->body.len);
    }
    else
    {
        event.id = CIOT_HTTPS_EVENT_DATA;
        event.msg.id = this->iface.id;
        event.msg.iface = this->iface.base.type;
        event.msg.type = CIOT_MSG_TYPE_UNKOWN;
        event.msg.data.https.msg.url = (char *)hm->uri.ptr;
        event.msg.data.https.msg.method = (char *)hm->method.ptr;
        event.msg.data.https.msg.data = (uint8_t *)hm->body.ptr;
        event.msg.data.https.msg.size = hm->body.len;
        event.size = CIOT_MSG_GET_SIZE(event.msg.data.https.msg);
    }
    this->conn_tx = c;
    this->iface.event_handler(this, &event, this->iface.event_args);
}

static void ciot_https_event_handle(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    ciot_https_t this = fn_data;

    switch (ev)
    {
    case MG_EV_HTTP_MSG:
        if (this->iface.event_handler != NULL)
        {
            struct mg_http_message *hm = ev_data, tmp = {0};
            mg_http_parse((char *)c->recv.buf, c->recv.len, &tmp);
            ciot_https_on_msg(this, c, hm);
        }
        else
        {
            mg_http_reply(this->conn_rx, 500, NULL, "Event Handler is NULL");
        }
        break;
    default:
        break;
    }
    (void)fn_data;
}
