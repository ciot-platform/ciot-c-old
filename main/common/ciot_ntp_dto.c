/**
 * @file ciot_ntp_dto.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_ntp.h"

ciot_err_t ciot_ntp_config_from_json(CJSON_PARAMETERS(ciot_ntp_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(op_mode);
    CJSON_GET_NUMBER(sync_mode);
    CJSON_GET_NUMBER(sync_interval);
    CJSON_GET_CHAR_ARRAY(server1);
    CJSON_GET_CHAR_ARRAY(server2);
    CJSON_GET_CHAR_ARRAY(server3);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_config_to_json(CJSON_PARAMETERS(ciot_ntp_config_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(op_mode);
    CJSON_ADD_NUMBER(sync_mode);
    CJSON_ADD_NUMBER(sync_interval);
    CJSON_ADD_CHAR_ARRAY(server1);
    CJSON_ADD_CHAR_ARRAY(server2);
    CJSON_ADD_CHAR_ARRAY(server3);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_info_from_json(CJSON_PARAMETERS(ciot_ntp_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_NUMBER(sync_count);
    CJSON_GET_NUMBER(last_sync);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_info_to_json(CJSON_PARAMETERS(ciot_ntp_info_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_NUMBER(sync_count);
    CJSON_ADD_NUMBER(last_sync);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_status_from_json(CJSON_PARAMETERS(ciot_ntp_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_GET_BOOLEAN(init);
    CJSON_GET_BOOLEAN(sync);
    CJSON_GET_NUMBER(state);
    return CIOT_ERR_OK;
}

ciot_err_t ciot_ntp_status_to_json(CJSON_PARAMETERS(ciot_ntp_status_t))
{
    CJSON_CHECK_PARAMETERS();
    CJSON_ADD_BOOLEAN(init);
    CJSON_ADD_BOOLEAN(sync);
    CJSON_ADD_NUMBER(state);
    return CIOT_ERR_OK;
}
