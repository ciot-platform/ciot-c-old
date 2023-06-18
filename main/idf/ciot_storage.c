/**
 * @file ciot_storage.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "nvs_flash.h"

#include "esp_partition.h"

#include "ciot_storage.h"
#include "ciot_err.h"

#define CIOT_STORAGE_NVS_NAMESPACE "ciot"  


static bool nvs_initialized = false;

ciot_err_t ciot_storage_init(void)
{
    if(nvs_initialized == false)
    {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        nvs_initialized = ret == ESP_OK;
        return ret;
    }
    else
    {
        return CIOT_ERR_OK;
    }
}

ciot_err_t ciot_storage_save_data(void *data, size_t size, char *name)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(CIOT_STORAGE_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(nvs_handle, name, data, size);
    if (err == ESP_OK) {
        err = nvs_commit(nvs_handle);
    }

    nvs_close(nvs_handle);
    return err;
}

ciot_err_t ciot_storage_load_data(void *data, size_t size, char *name)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(CIOT_STORAGE_NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t required_size = size;
    err = nvs_get_blob(nvs_handle, name, data, &required_size);

    nvs_close(nvs_handle);
    return err;
}

ciot_err_t ciot_storage_remove_data(char *name)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(CIOT_STORAGE_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_erase_key(nvs_handle, name);
    if (err != ESP_OK) {
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
    return err;
}

size_t ciot_storage_get_size(void)
{
    const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "nvs");
    return partition->size;
}
