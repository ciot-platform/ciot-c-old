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

#include <stddef.h>
#include <stdio.h>
#include <wchar.h>

#include <windows.h>
#include "ciot_storage.h"

#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif

ciot_err_t ciot_storage_init(void)
{
    return CIOT_ERR_NOTHING_TO_DO;
}

ciot_err_t ciot_storage_save_data(void *data, size_t size, char *name)
{
    FILE *f = fopen(name, "w");

    if (f == NULL)
    {
        return CIOT_ERR_NOT_FOUND;
    }
    else
    {
        fwrite(data, size, 1, f);
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_storage_load_data(void *data, size_t size, char *name)
{
    FILE *f = fopen(name, "r");

    if (f == NULL)
    {
        return CIOT_ERR_NOT_FOUND;
    }
    else
    {
        fread(data, size, 1, f);
    }

    return CIOT_ERR_OK;
}

ciot_err_t ciot_storage_remove_data(char *name)
{
    if (remove(name) == 0)
    {
        return CIOT_ERR_FAIL;
    }
    else
    {
        return CIOT_ERR_OK;
    }
}

size_t ciot_storage_get_size()
{
    char modulePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, modulePath, MAX_PATH) != 0)
    {
        char drive[_MAX_DRIVE];
        if (_splitpath_s(modulePath, drive, _MAX_DRIVE, NULL, 0, NULL, 0, NULL, 0) == 0)
        {
            ULONGLONG freeBytesAvailable, totalBytes, totalFreeBytes;
            if (GetDiskFreeSpaceExA(drive, (PULARGE_INTEGER)&freeBytesAvailable, (PULARGE_INTEGER)&totalBytes, (PULARGE_INTEGER)&totalFreeBytes))
            {
                return freeBytesAvailable / 1024;
            }
            else
            {
                printf("Failed to retrieve disk space information.\n");
            }
        }
        else
        {
            printf("Failed to extract drive path.\n");
        }
    }
    else
    {
        printf("Failed to retrieve module file name.\n");
    }

    return 0;
}