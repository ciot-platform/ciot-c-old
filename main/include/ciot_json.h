/**
 * @file ciot_json.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __CIOT_JSON__H__
#define __CIOT_JSON__H__

#include <string.h>
#include <stdbool.h>

#include "cJSON.h"

#include "ciot_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CJSON_PARAMETERS(type_t) cJSON *json, type_t *ptr

#define CJSON_CREATE_INTEGER(x)                    \
    cJSON j_##x * = cJSON_GetObjectItem(json, #x); \
    if (j_##x && j_##x->type == cJSON_Number)      \
    int x = j_##x->valueint

#define CJSON_CREATE_ENUM(x, type_t)              \
    cJSON *j_##x = cJSON_GetObjectItem(json, #x); \
    type_t x = 0;                                 \
    if (j_##x && j_##x->type == cJSON_Number)     \
    x = (type_t)j_##x->valueint

#define CJSON_CHECK_PARAMETERS()     \
    if (json == NULL || ptr == NULL) \
        return CIOT_ERR_INVALID_ARG;

#define CJSON_GET_BOOLEAN(x)                  \
    cJSON *x = cJSON_GetObjectItem(json, #x); \
    if (x)                                    \
        ptr->x = x->type == cJSON_True;       \
    else                                      \
        ptr->x = false

#define CJSON_GET_BOOLEAN_UNION_CHILD(x, union, received_type, expected_type) \
    if (ptr->received_type == received_type)                               \
    {                                                                      \
        cJSON *x = cJSON_GetObjectItem(json, #x);                          \
        if (x)                                                             \
            ptr->union.x = x->type == cJSON_True;                          \
        else                                                               \
            ptr->union.x = false;                                          \
    }

#define CJSON_GET_NUMBER(x)                   \
    cJSON *x = cJSON_GetObjectItem(json, #x); \
    if (x && x->type == cJSON_Number)         \
    ptr->x = x->valueint

#define CJSON_GET_NUMBER_UNION_CHILD(x, received_type, expected_type) \
    if (received_type == expected_type)                            \
    {                                                              \
        cJSON *x = cJSON_GetObjectItem(json, #x);                  \
        if (x && x->type == cJSON_Number)                          \
            ptr->x = x->valueint;                                  \
    }

#define CJSON_GET_NUMBER_ARRAY(x)                           \
    cJSON *x = cJSON_GetObjectItem(json, #x);               \
    for (size_t i = 0; i < sizeof(ptr->x); i++)             \
    {                                                       \
        if (i < cJSON_GetArraySize(x))                      \
        {                                                   \
            ptr->x[i] = cJSON_GetArrayItem(x, i)->valueint; \
        }                                                   \
    }

#define CJSON_GET_NUMBER_ARRAY_UNION_CHILD(x, union, received_type, expected_type) \
    if (ptr->received_type == received_type)                                    \
    {                                                                           \
        cJSON *x = cJSON_GetObjectItem(json, #x);                               \
        if (x)                                                                  \
        {                                                                       \
            for (size_t i = 0; i < sizeof(ptr->.union.x); i++)                  \
            {                                                                   \
                if (i < cJSON_GetArraySize(x))                                  \
                {                                                               \
                    ptr->.union.x[i] = cJSON_GetArrayItem(x, i)->valueint;      \
                }                                                               \
            }                                                                   \
        }                                                                       \
    }

#define CJSON_GET_CHAR_ARRAY(x)                                    \
    cJSON *x = cJSON_GetObjectItem(json, #x);                      \
    if (x && x->type == cJSON_String && x->valuestring[0] != '\0') \
    {                                                              \
        size_t len = strnlen(x->valuestring, sizeof(ptr->x));      \
        strncpy((char *)ptr->x, x->valuestring, len);              \
        ptr->x[len] = '\0';                                        \
    }

#define CJSON_GET_CHAR_ARRAY_UNION_CHILD(x, union, received_type, expected_type) \
    if (ptr->received_type == received_type)                                  \
    {                                                                         \
        cJSON *x = cJSON_GetObjectItem(json, #x);                             \
        if (x && x->type == cJSON_String && x->valuestring[0] != '\0')        \
        {                                                                     \
            size_t len = strnlen(x->valuestring, sizeof(ptr->union.x));       \
            strncpy((char *)ptr->union.x, x->valuestring, len);               \
            ptr->union.x[len] = '\0';                                         \
        }                                                                     \
    }

#define CJSON_GET_STRING(x)                                        \
    cJSON *x = cJSON_GetObjectItem(json, #x);                      \
    if (x && x->type == cJSON_String && x->valuestring[0] != '\0') \
    {                                                              \
        ptr->x = malloc(strlen(x->valuestring) + 1);               \
        strcpy((char *)ptr->x, x->valuestring);                    \
    }

#define CJSON_ADD_BOOLEAN(x) cJSON_AddBoolToObject(json, #x, ptr->x)

#define CJSON_ADD_NUMBER(x) cJSON_AddNumberToObject(json, #x, ptr->x)

#define asizeof(x) sizeof(x) / sizeof(x[0])

#define CJSON_ADD_NUMBER_ARRAY(x)                                       \
    {                                                                   \
        size_t count = 0;                                               \
        for (size_t i = 0; i < asizeof(ptr->x); i++)                    \
        {                                                               \
            if (ptr->x[i] != 0)                                         \
                break;                                                  \
            count++;                                                    \
        }                                                               \
        if (count != (asizeof(ptr->x)))                                 \
        {                                                               \
            cJSON *x = cJSON_CreateArray();                             \
            for (size_t i = 0; i < asizeof(ptr->x); i++)                \
            {                                                           \
                cJSON_AddItemToArray(x, cJSON_CreateNumber(ptr->x[i])); \
            }                                                           \
            cJSON_AddItemToObject(json, #x, x);                         \
        }                                                               \
    }

#define CJSON_ADD_CHAR_ARRAY(x)                                 \
    if (ptr->x != NULL && strnlen(ptr->x, sizeof(ptr->x)) != 0) \
    cJSON_AddStringToObject(json, #x, (char *)ptr->x)

#define CJSON_ADD_HEX_ARRAY(x)                    \
    char x[(sizeof(ptr->x) * 2) + 1];             \
    int len = 0;                                  \
    for (size_t i = 0; i < sizeof(ptr->x); i++)   \
    {                                             \
        len += sprintf(&x[len], "%X", ptr->x[i]); \
    }                                             \
    x[len] = '\0';                                \
    cJSON_AddStringToObject(json, #x, x);

#define CJSON_GET_OBJ(obj, handler)                                    \
    {                                                                  \
        int err = handler(cJSON_GetObjectItem(json, #obj), &ptr->obj); \
        if (err != CIOT_ERR_OK)                                        \
            return err;                                                \
    }

#define CJSON_GET_OBJ_FROM_ROOT(obj, handler) handler(json, &ptr->obj)

#define CJSON_GET_OBJ_UNION_CHILD(obj, handler, received_type, expected_type) \
    if (received_type == expected_type)                                    \
    {                                                                      \
        int err = handler(json, &ptr->obj);                                \
        if (err != CIOT_ERR_OK)                                            \
            return err;                                                    \
    }

#define CJSON_GET_UNION_CHILD_OF_UNION(obj, handler, child_type, received_type, expected_type) \
    if (received_type == expected_type)                                                  \
    {                                                                                    \
        int err = handler(json, &ptr->obj, child_type);                                  \
        if (err != CIOT_ERR_OK)                                                          \
            return err;                                                                  \
    }

#define CJSON_GET_UNION(obj, handler, type)            \
    {                                                  \
        int err = handler(json, &ptr->obj, ptr->type); \
        if (err != CIOT_ERR_OK)                        \
            return err;                                \
    }

#define CJSON_ADD_OBJ(obj, handler)             \
    if (handler != NULL)                        \
    {                                           \
        cJSON *obj = cJSON_CreateObject();      \
        int err = handler(obj, &ptr->obj);      \
        if (err != CIOT_ERR_OK)                 \
            return err;                         \
        cJSON_AddItemToObject(json, #obj, obj); \
    }                                           \
    else                                        \
    {                                           \
        return CIOT_ERR_INVALID_ARG;            \
    }

#define CJSON_ADD_OBJ_TO_ROOT(obj, handler) handler(json, &ptr->obj);

#define CJSON_ADD_OBJ_ARRAY(obj, handler, size)             \
    if (handler != NULL)                                      \
    {                                                         \
        if (size > 0)                                         \
        {                                                     \
            cJSON *array = cJSON_CreateArray();               \
            for (size_t i = 0; i < size; i++)                 \
            {                                                 \
                cJSON *item = cJSON_CreateObject();           \
                ciot_err_t err = handler(item, &ptr->obj[i]); \
                if (err == CIOT_ERR_OK)                       \
                {                                             \
                    cJSON_AddItemToArray(array, item);        \
                }                                             \
                else                                          \
                {                                             \
                    return err;                               \
                }                                             \
            }                                                 \
            cJSON_AddItemToObject(json, #obj, array);         \
        }                                                     \
    }                                                         \
    else                                                      \
    {                                                         \
        return CIOT_ERR_INVALID_ARG;                          \
    }

#define CJSON_ADD_OBJ_UNION_CHILD(obj, handler, received_type, expected_type) \
    if (received_type == expected_type)                                          \
    {                                                                            \
        int err = handler(json, &ptr->obj);                                      \
        if (err != CIOT_ERR_OK)                                                  \
            return err;                                                          \
    }

#define CJSON_ADD_UNION_CHILD_OF_UNION(obj, handler_parent, parent_received_type, parent_expected_type, child_received_type) \
    if (parent_received_type == parent_expected_type)                                                                        \
    {                                                                                                                        \
        int err = handler_parent(json, &ptr->obj, child_received_type);                                                      \
        if (err != CIOT_ERR_OK)                                                                                              \
            return err;                                                                                                      \
    }

#define CJSON_ADD_UNION(obj, handler, type)           \
    if (handler != NULL)                              \
    {                                                 \
        cJSON *obj = cJSON_CreateObject();            \
        int err = handler(obj, &ptr->obj, ptr->type); \
        if (err != CIOT_ERR_OK)                       \
            return err;                               \
        cJSON_AddItemToObject(json, #obj, obj);       \
    }                                                 \
    else                                              \
    {                                                 \
        return CIOT_ERR_INVALID_ARG;                  \
    }

#define CJSON_ADD_UNION_TO_ROOT(obj, handler, type)    \
    if (handler != NULL)                               \
    {                                                  \
        int err = handler(json, &ptr->obj, ptr->type); \
        if (err != CIOT_ERR_OK)                        \
            return err;                                \
    }                                                  \
    else                                               \
    {                                                  \
        return CIOT_ERR_INVALID_ARG;                   \
    }

#ifdef __cplusplus
}
#endif

#endif //!__CIOT_JSON__H__