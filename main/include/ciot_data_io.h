/**
 * @file ciot_data_io.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_DATA_IO__H__
#define __CIOT_DATA_IO__H__

#include <stddef.h>

#include "ciot_msg_dto.h"
#include "ciot_err.h"

typedef enum ciot_data_type
{
    CIOT_DATA_TYPE_UNKNOWN,
    CIOT_DATA_TYPE_RAW,
    CIOT_DATA_TYPE_JSON_STRING,
    CIOT_DATA_TYPE_
} ciot_data_type_t;

typedef struct ciot_data_buffer
{
    size_t size;
    void *content;
} ciot_data_buffer_t;

typedef struct ciot_data
{
    ciot_data_type_t data_type;
    ciot_data_buffer_t buffer;
} ciot_data_t;

ciot_err_t ciot_data_deserialize_msg(ciot_data_t *input, ciot_msg_t *output);
ciot_err_t ciot_data_serialize_resp(ciot_msg_response_t *input, ciot_data_t *output);

#endif  //!__CIOT_DATA_IO__H__