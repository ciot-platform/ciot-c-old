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

#include <stdio.h>

#include "ciot_config.h"

#if CIOT_CONFIG_CONSOLE_ERROR_MESSAGES == 0

#define CIOT_ERROR_PRINT(x) x
#define CIOT_ERROR_LOG(x) while (0)

#else
#if (CIOT_CONFIG_API_ERROR_MESSAGES)

#define CIOT_ERROR_LOG(x) printf("%s:%d error: %s\n", __FILE__, __LINE__, ciot_err_to_message(hc_err))
#define CIOT_ERROR_PRINT(x)                                                               \
    do                                                                                    \
    {                                                                                     \
        ciot_err_t hc_err = x;                                                            \
        if (hc_err != CIOT_ERR_OK)                                                        \
        {                                                                                 \
            printf("%s:%d error: %s\n", __FILE__, __LINE__, ciot_err_to_message(hc_err)); \
        }                                                                                 \
    } while (0)

#else

#define CIOT_ERROR_LOG(x) printf("%s:%d\n", __FILE__, __LINE__)
#define CIOT_ERROR_PRINT(x)                               \
    do                                                    \
    {                                                     \
        ciot_err_t hc_err = x;                            \
        if (hc_err != CIOT_ERR_OK)                        \
        {                                                 \
            printf("%s:%d:%s\n", __FILE__, __LINE__, #x); \
        }                                                 \
    } while (0)
#endif

#endif

#define CIOT_ERROR_RETURN(x)        \
    do                              \
    {                               \
        ciot_err_t hc_err = x;      \
        if (hc_err != CIOT_ERR_OK)  \
        {                           \
            CIOT_ERROR_LOG(hc_err); \
            return hc_err;          \
        }                           \
    } while (0)

typedef enum ciot_err
{
    CIOT_ERR_FAIL = -1,
    CIOT_ERR_OK,
    CIOT_ERR_TIMEOUT,
    CIOT_ERR_NOT_FOUND,
    CIOT_ERR_NOT_IMPLEMENTED,
    CIOT_ERR_NOTHING_TO_DO,
    CIOT_ERR_FEATURE_NOT_SUPPORTED,
    CIOT_ERR_BUSY,
    CIOT_ERR_INVALID_ARG,
    CIOT_ERR_INVALID_ID,
    CIOT_ERR_INVALID_TYPE,
    CIOT_ERR_INVALID_INTERFACE,
    CIOT_ERR_INVALID_REQUEST,
    CIOT_ERR_INVALID_STATE,
    CIOT_ERR_MEMORY_ALLOCATION,
    CIOT_ERR_MEMORY_OVERFLOW,
    CIOT_ERR_NULL_REFERENCE,
} ciot_err_t;

const char *ciot_err_to_message(ciot_err_t err);

#endif //!__CIOT_ERR__H__