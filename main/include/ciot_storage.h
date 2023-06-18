/**
 * @file ciot_nvs.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_NVS__H__
#define __CIOT_NVS__H__

#include <inttypes.h>

#include "ciot_err.h"

ciot_err_t ciot_storage_init(void);
ciot_err_t ciot_storage_save_data(void *data, size_t size, char *name);
ciot_err_t ciot_storage_load_data(void *data, size_t size, char *name);
ciot_err_t ciot_storage_remove_data(char *name);
size_t ciot_storage_get_size(void);

#endif  //!__CIOT_NVS__H__