/**
 * @file ciot_system.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <windows.h>

#include "ciot_system.h"
#include "ciot_storage.h"
#include "ciot_config.h"

static DWORDLONG ciot_system_get_free_ram();
static unsigned long ciot_system_get_lifetime();

ciot_err_t ciot_system_get_status(ciot_system_status_t *status)
{
    ciot_system_status_t system = {
        .time = time(NULL),
        .memory = ciot_system_get_free_ram(),
        .err = ciot_system_get_err_code(),
        .status = ciot_system_get_status_code(),
        .lifetime = ciot_system_get_lifetime()
    };
    *status = system;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_get_info(ciot_system_info_t *info)
{
    ciot_system_info_t system = {
        .device = CIOT_CONFIG_DEVICE_TYPE,
        .os = CIOT_SYSTEM_OS_FREERTOS,
        .version = CIOT_CONFIG_VERSION,
        .date = CIOT_CONFIG_DATE,
        .board = CIOT_CONFIG_BOARD,
        .mcu = CIOT_CONFIG_MCU,
        .storage = ciot_storage_get_size(),
        .features = CIOT_CONFIG_FEATURES,
    };
    *info = system;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_reset(void)
{
#if _WIN32
    system("shutdown /r /t 0");
#else
    system("sudo reboot");
#endif
    return CIOT_ERR_OK;
}

static DWORDLONG ciot_system_get_free_ram()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullAvailPhys / 1024;
}

static unsigned long ciot_system_get_lifetime()
{
    DWORD ticks = GetTickCount();
    return ticks / 1000;
}