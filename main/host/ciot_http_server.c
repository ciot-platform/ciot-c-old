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

#include "mongoose.h"

#include "ciot_app.h"
#include "ciot_http_server.h"

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

static void ciot_http_server_on_post(struct mg_connection *c, struct mg_http_message *mg)
{
    struct mg_str *content_type = mg_http_get_header(mg, "Content-Type");
    ciot_msg_t msg = { 0 };
    ciot_msg_response_t resp = { 0 };
    ciot_err_t err;

    if(content_type->len != 0)
    {
        if(strncmp(content_type->ptr, CIOT_HTTP_CONTENT_TYPE_JSON, content_type->len) == 0)
        {
            cJSON *json = cJSON_Parse(mg->body.ptr);
            err = ciot_msg_from_json(json, &msg);
        }
        else
        {
            char err_msg[64];
            sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK_WITH_MSG, err, "Content-Type not supported");
            mg_http_reply(c, 400, NULL, err_msg);
        }

        if(err != CIOT_ERR_OK)
        {
            char err_msg[28];
            sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK, err);
            mg_http_reply(c, 400, NULL, err_msg);
        }

        err = ciot_app_send_msg(&msg);
        if(err == CIOT_ERR_OK)
        {
            ciot_app_wait_process(20000);
            ciot_app_get_msg_response(&resp);
            cJSON *json = cJSON_CreateObject();
            ciot_msg_response_to_json(json, &resp);
            char *json_str = cJSON_Print(json);
            if(resp.err_code == CIOT_ERR_OK)
            {
                mg_http_reply(c, 200, NULL, json_str);
            }
            else
            {
                mg_http_reply(c, 500, NULL, json_str);
            }
            cJSON_Delete(json);
            free(json_str);
        }

    }
    else
    {
        char err_msg[64];
        sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK_WITH_MSG, CIOT_ERR_INVALID_REQUEST, "Missing Content-Type");
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
        mg_http_parse((char*) c->recv.buf, c->recv.len, &tmp);
        bool is_post = strncmp(hm->method.ptr, "POST", hm->method.len) == 0;
        if(mg_http_match_uri(hm, this.endpoint) && is_post)
        {
            ciot_http_server_on_post(c, hm);
        }
        else
        {
            char error_msg[64];
            sprintf(error_msg, CIOT_HTTP_SERVER_ERROR_MASK_WITH_MSG, CIOT_ERR_NOT_FOUND, "Not Found");
            mg_http_reply(c, 404, NULL, error_msg);
        }
        break;
    }
    default:
        break;
    }
    (void) fn_data;
}
