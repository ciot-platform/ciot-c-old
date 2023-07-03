/**
 * @file ciot_data_io.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h>

#include "cJSON.h"
#include "ciot_msg_dto.h"
#include "ciot_data_io.h"

ciot_err_t ciot_data_deserialize_msg(ciot_data_t *input, ciot_msg_t *output)
{
    switch (input->data_type)
    {
    case CIOT_DATA_TYPE_RAW:
    {
        if(input->buffer.size > sizeof(*output))
        {
            return CIOT_ERR_MEMORY_OVERFLOW;
        }
        memcpy(output, input->buffer.content, input->buffer.size);
        return CIOT_ERR_OK;
    }
    case CIOT_DATA_TYPE_JSON_STRING:
    {
        cJSON *json = cJSON_Parse(input->buffer.content);
        ciot_err_t err = ciot_msg_from_json(json, output);
        free(json);
        return err;
    }
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}

ciot_err_t ciot_data_serialize_resp(ciot_msg_response_t *input, ciot_data_t *output)
{
    switch (output->data_type)
    {
    case CIOT_DATA_TYPE_RAW:
    {
        if(output->buffer.size < sizeof(*input))
        {
            return CIOT_ERR_MEMORY_OVERFLOW;
        }
        memcpy(output->buffer.content, input, sizeof(*input));
        return CIOT_ERR_OK;
    }
    case CIOT_DATA_TYPE_JSON_STRING:
    {
        cJSON *json = cJSON_CreateObject();
        ciot_err_t err = ciot_msg_response_to_json(json, input);
        if(err == CIOT_ERR_OK)
        {
            output->buffer.content = cJSON_PrintUnformatted(json);
            output->buffer.size = strlen(output->buffer.content);
        }
        free(json);
        return err;
    }
    default:
        return CIOT_ERR_INVALID_TYPE;
    }
}
