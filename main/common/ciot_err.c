/**
 * @file ciot_err.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_err.h"
#include "ciot_config.h"

#if CIOT_CONFIG_API_ERROR_MESSAGES

#ifdef IDF_VER
#include "esp_err.h"
#endif

const char *ciot_err_to_message(ciot_err_t err)
{
    switch (err)
    {
    case CIOT_ERR_FAIL:
        return "Fail";
    case CIOT_ERR_OK:
        return "OK";
    case CIOT_ERR_TIMEOUT:
        return "Timeout";
    case CIOT_ERR_NOT_FOUND:
        return "Not Found";
    case CIOT_ERR_NOT_IMPLEMENTED:
        return "Not Implemented";
    case CIOT_ERR_NOTHING_TO_DO:
        return "Nothing to do";
    case CIOT_ERR_FEATURE_NOT_SUPPORTED:
        return "Feature Not Supported";
    case CIOT_ERR_BUSY:
        return "Busy";
    case CIOT_ERR_INVALID_ARG:
        return "Invalid Argument";
    case CIOT_ERR_INVALID_ID:
        return "Invalid ID";
    case CIOT_ERR_INVALID_TYPE:
        return "Invalid Type";
    case CIOT_ERR_INVALID_INTERFACE:
        return "Invalid Interface";
    case CIOT_ERR_INVALID_REQUEST:
        return "Invalid Request";
    case CIOT_ERR_INVALID_STATE:
        return "Invalid State";
    case CIOT_ERR_INVALID_VERSION:
        return "Invalid Version";
    case CIOT_ERR_SAME_VERSION:
        return "Same Version";
    case CIOT_ERR_INVALID_SIZE:
        return "Invalid Size";
    case CIOT_ERR_MEMORY_ALLOCATION:
        return "Memory Allocation Error";
    case CIOT_ERR_MEMORY_OVERFLOW:
        return "Memory Overflow Error";
    case CIOT_ERR_NULL_REFERENCE:
        return "Null Reference";
    case CIOT_ERR_INIT:
        return "Initialization Error";
    case CIOT_ERR_READ:
        return "Read Operation Error";
    default:
#ifdef IDF_VER
    return esp_err_to_name(err);
#else
    return "Unknown Error";
#endif
    }
}

#endif
