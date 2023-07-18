/**
 * @file ciot_ota_dto.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_config.h"

#if CIOT_CONFIG_FEATURE_OTA

#include "ciot_ota.h"

ciot_err_t ciot_ota_set_config(ciot_ota_t *ota, ciot_ota_config_t *conf)
{
    ota->config = *conf;
    return ciot_ota_start(ota);
}

ciot_err_t ciot_ota_get_config(ciot_ota_t *ota, ciot_ota_config_t *conf)
{
    *conf = ota->config;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_get_status(ciot_ota_t *ota, ciot_ota_status_t *status)
{
    *status = ota->status;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_get_info(ciot_ota_t *ota, ciot_ota_info_t *info)
{
    *info = ota->info;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_set_callbacks(ciot_ota_t *ota, ciot_ota_callbacks_t *callbacks)
{
    ota->callbacks = *callbacks;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_config_from_json(CJSON_PARAMETERS(ciot_ota_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_CHAR_ARRAY(url);
    CJSON_GET_BOOLEAN(force);
    CJSON_GET_BOOLEAN(encrypted);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_config_to_json(CJSON_PARAMETERS(ciot_ota_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_CHAR_ARRAY(url);
    CJSON_ADD_BOOLEAN(force);
    CJSON_ADD_BOOLEAN(encrypted);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_info_from_json(CJSON_PARAMETERS(ciot_ota_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER_ARRAY(sha256);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_info_to_json(CJSON_PARAMETERS(ciot_ota_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER_ARRAY(sha256);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_status_from_json(CJSON_PARAMETERS(ciot_ota_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(state);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ota_status_to_json(CJSON_PARAMETERS(ciot_ota_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(state);
    return CIOT_ERR_OK;
}

#endif