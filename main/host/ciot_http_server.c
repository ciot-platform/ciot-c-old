/**
 * @file ciot_http_server.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ciot_config.h"
#include "ciot_http_server.h"

#if CIOT_CONFIG_FEATURE_HTTP_SERVER

#include "mongoose.h"

#include "ciot_app.h"
#include "ciot_data_io.h"

typedef struct ciot_http_server
{
    char endpoint[48];
} ciot_http_server_t;

extern struct mg_mgr mgr;
static ciot_http_server_t this;

static void ciot_http_server_event_handle(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

ciot_err_t ciot_http_server_start(ciot_http_server_config_t *conf)
{
    struct mg_connection *c;
    char address[128];
    sprintf(address, "http://0.0.0.0:%d%s", conf->port, conf->endpoint);
    memcpy(&this.endpoint, &conf->endpoint, sizeof(this.endpoint));
    if ((c = mg_http_listen(&mgr, address, ciot_http_server_event_handle, &mgr)) == NULL)
    {
        return CIOT_ERR_FAIL;
    }
    printf("Server Started on port %d\n", conf->port);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_http_server_stop(void)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_http_server_set_response(ciot_msg_t *msg)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_data_type_t ciot_http_server_get_data_type(struct mg_str *content_type)
{
    if (strncmp(content_type->ptr, CIOT_HTTP_CONTENT_TYPE_JSON, content_type->len) == 0)
    {
        return CIOT_DATA_TYPE_JSON_STRING;
    }
    else if (strncmp(content_type->ptr, CIOT_HTTP_CONTENT_TYPE_OCTET, content_type->len) == 0)
    {
        return CIOT_DATA_TYPE_RAW;
    }
    else
    {
        return CIOT_DATA_TYPE_UNKNOWN;
    }
}

static void ciot_http_server_on_post(struct mg_connection *c, struct mg_http_message *mg)
{
    struct mg_str *content_type = mg_http_get_header(mg, "Content-Type");
    ciot_err_t err;

    if (content_type->len != 0)
    {
        ciot_data_type_t data_type = ciot_http_server_get_data_type(content_type);
        if (data_type == CIOT_DATA_TYPE_UNKNOWN)
        {
            char err_msg[64];
            err = CIOT_ERR_INVALID_TYPE;
            sprintf(err_msg, CIOT_ERROR_MASK_WITH_MSG, err, "Content-Type not supported");
            mg_http_reply(c, 400, NULL, err_msg);
        }

        ciot_msg_t msg = {0};
        ciot_data_t msg_data = {
            .data_type = data_type,
            .buffer.content = (void *)mg->body.ptr,
            .buffer.size = mg->body.len,
        };
        err = ciot_data_deserialize_msg(&msg_data, &msg);
        if (err != CIOT_ERR_OK)
        {
            char err_msg[28];
            sprintf(err_msg, CIOT_ERROR_MASK, err);
            mg_http_reply(c, 400, NULL, err_msg);
        }
        else
        {
            err = ciot_app_send_msg(&msg);
            if (err == CIOT_ERR_OK)
            {
                ciot_data_t resp_data = { .data_type = data_type };
                ciot_msg_response_t resp = {0};
                ciot_app_wait_process(20000);
                ciot_app_get_msg_response(&resp);

                err = ciot_data_serialize_resp(&resp, &resp_data);
                if (err != CIOT_ERR_OK)
                {
                    char err_msg[28];
                    sprintf(err_msg, CIOT_ERROR_MASK, err);
                    mg_http_reply(c, 400, NULL, err_msg);
                }
                else
                {
                    int status_code = resp.err == CIOT_ERR_OK ? 200 : 500;
                    mg_http_reply(c, status_code, NULL, resp_data.buffer.content);
                    free(resp_data.buffer.content);
                }
            }
        }
    }
    else
    {
        char err_msg[64];
        sprintf(err_msg, CIOT_ERROR_MASK_WITH_MSG, CIOT_ERR_INVALID_REQUEST, "Missing Content-Type");
        mg_http_reply(c, 400, NULL, err_msg);
    }
}

static void ciot_http_server_event_handle(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    switch (ev)
    {
    case MG_EV_HTTP_MSG:
    {
        struct mg_http_message *hm = ev_data, tmp = {0};
        mg_http_parse((char *)c->recv.buf, c->recv.len, &tmp);
        bool is_post = strncmp(hm->method.ptr, "POST", hm->method.len) == 0;
        if (mg_http_match_uri(hm, this.endpoint) && is_post)
        {
            ciot_http_server_on_post(c, hm);
        }
        else
        {
            char error_msg[64];
            sprintf(error_msg, CIOT_ERROR_MASK_WITH_MSG, CIOT_ERR_NOT_FOUND, "Not Found");
            mg_http_reply(c, 404, NULL, error_msg);
        }
        break;
    }
    default:
        break;
    }
    (void)fn_data;
}

#endif