/**
 * @file ciot_err.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_ERR__H__
#define __CIOT_ERR__H__

typedef enum ciot_err
{
    CIOT_ERR_FAIL=-1,
    CIOT_ERR_OK,
    CIOT_ERR_NOT_IMPLEMENTED,
    CIOT_ERR_NOT_SUPPORTED,
    CIOT_ERR_BUSY,
    CIOT_ERR_INVALID_ARG,
    CIOT_ERR_INVALID_ID,
    CIOT_ERR_INVALID_TYPE,
    CIOT_ERR_INVALID_INTERFACE,
    CIOT_ERR_INVALID_REQUEST,
    CIOT_ERR_MEMORY_ALLOCATION,
    CIOT_ERR_NULL_REFERENCE,
} ciot_err_t;

#endif  //!__CIOT_ERR__H__