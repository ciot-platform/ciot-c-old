/**
 * @file ciot_http_server.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "esp_http_server.h"
#include "esp_log.h"

#include "ciot_http_server.h"
#include "ciot_app.h"

typedef struct ciot_http_server
{
    httpd_handle_t handle;
    char endpoint[48];
    char buffer[512];
} ciot_http_server_t;

static ciot_http_server_t this;
static const char *TAG = "ciot_http_server";

static void ciot_http_server_register_routes();
static esp_err_t ciot_http_server_on_post(httpd_req_t *req);

ciot_err_t ciot_http_server_start(ciot_http_server_config_t *conf)
{
    httpd_config_t httpd = HTTPD_DEFAULT_CONFIG();
    httpd.server_port = conf->port;
    memcpy(&this.endpoint, &conf->endpoint, sizeof(this.endpoint));
    
    esp_err_t err = httpd_start(&this.handle, &httpd);
    if(err == ESP_OK)
    {
        ciot_http_server_register_routes();
        ESP_LOGI(TAG, "Server Started on port %d", httpd.server_port);
    }

    return err;
}

ciot_err_t ciot_http_server_stop(void)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

ciot_err_t ciot_http_server_set_response(ciot_msg_t *msg)
{
    return CIOT_ERR_NOT_IMPLEMENTED;
}

static void ciot_http_server_register_routes()
{
    httpd_uri_t ciot_http_server_post = {
        .uri = this.endpoint,
        .handler = ciot_http_server_on_post,
        .method = HTTP_POST
    };
    httpd_register_uri_handler(this.handle, &ciot_http_server_post);
}

static esp_err_t ciot_http_server_on_post(httpd_req_t *req)
{
    char content_type[100];
    ciot_msg_t msg = { 0 };
    ciot_msg_response_t resp = { 0 };
    esp_err_t err = CIOT_ERR_OK;

    CIOT_ERROR_RETURN(httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*"));

    if(httpd_req_get_hdr_value_str(req, "Content-Type", content_type, sizeof(content_type)) == ESP_OK)
    {
        if(strcmp(content_type, HTTPD_TYPE_OCTET) == 0)
        {
            httpd_req_recv(req, (char*)&msg, sizeof(msg));
        }
        else if(strcmp(content_type, HTTPD_TYPE_JSON) == 0)
        {
            httpd_req_recv(req, this.buffer, sizeof(this.buffer));
            cJSON *json = cJSON_Parse(this.buffer);
            err = ciot_msg_from_json(json, &msg);
        }
        else
        {
            char err_msg[64];
            sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK_WITH_MSG, err, "Content-Type not supported");
            return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err_msg);
        }

        if(err != ESP_OK)
        {
            char err_msg[28];
            sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK, err);
            return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err_msg);
        }

        err = ciot_app_send_msg(&msg);
        if(err == ESP_OK)
        {
            ciot_app_wait_process(20000);
            ciot_app_get_msg_response(&resp);
            cJSON *json = cJSON_CreateObject();
            ciot_msg_response_to_json(json, &resp);
            char *json_str = cJSON_Print(json);
            if(resp.err_code == CIOT_ERR_OK)
            {
                err = httpd_resp_send(req, json_str, strlen(json_str));
            }
            else
            {
                err = httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, json_str);
            }
            cJSON_Delete(json);
            free(json_str);
            return err;
        }
        else
        {
            char err_msg[28];
            sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK, CIOT_ERR_INVALID_REQUEST);
            return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err_msg);
        }

    }
    else
    {
        char err_msg[64];
        sprintf(err_msg, CIOT_HTTP_SERVER_ERROR_MASK_WITH_MSG, err, "Missing Content-Type");
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err_msg);
    }
}
