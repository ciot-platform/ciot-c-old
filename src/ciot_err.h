/**
 * @file ciot_err.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_ERR__H__
#define __CIOT_ERR__H__

#define CIOT_ERR_BASE 0x0

#define CIOT_ERR_NULL_CHECK(x) if(x == NULL) return CIOT_ERR_NULL_ARG

typedef enum __attribute__((packed)) {
    CIOT_FAIL=-1,
    CIOT_OK,
    CIOT_ERR_NULL_ARG=CIOT_ERR_BASE,
    CIOT_ERR_INVALID_ID,
    CIOT_ERR_INVALID_TYPE,
    CIOT_ERR_OVERFLOW,
    CIOT_ERR_NOT_IMPLEMENTED,
    CIOT_ERR_NOT_SUPPORTED,
    CIOT_ERR_BUSY,
} ciot_err_t;

#endif  //!__CIOT_ERR__H__