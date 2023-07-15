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

#include <stdlib.h>

#ifdef  _WIN32
#include <windows.h>
#endif  //_WIN32

#include "ciot_system.h"
#include "ciot_storage.h"
#include "ciot_config.h"

static size_t ciot_system_get_free_ram();
static time_t ciot_system_get_lifetime();

ciot_err_t ciot_system_get_status(ciot_system_status_t *status)
{
    ciot_system_status_t system = {
        .time = time(NULL),
        .memory = ciot_system_get_free_ram(),
        .sys_err = ciot_system_get_err_code(),
        .sys_status = ciot_system_get_status_code(),
        .lifetime = ciot_system_get_lifetime()
    };
    *status = system;
    return CIOT_ERR_OK;
}

ciot_err_t ciot_system_get_info(ciot_system_info_t *info)
{
    ciot_system_info_t system = {
        .device = CIOT_CONFIG_DEVICE_TYPE,
        .os = CIOT_CONFIG_OS,
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

#ifdef  _WIN32
static size_t ciot_system_get_free_ram()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullAvailPhys / 1024;
}

static time_t ciot_system_get_lifetime()
{
    DWORD ticks = GetTickCount();
    return ticks / 1000;
}
#else
static size_t ciot_system_get_free_ram()
{
    FILE* meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL) {
        return 0;
    }

    char line[256];
    size_t free_ram = 0;

    while (fgets(line, sizeof(line), meminfo)) {
        if (sscanf(line, "MemAvailable: %zu kB", &free_ram) == 1) {
            break;
        }
    }

    fclose(meminfo);
    return free_ram;
}

static time_t ciot_system_get_lifetime()
{
    FILE* uptime_file = fopen("/proc/uptime", "r");
    if (uptime_file == NULL) {
        return 0;
    }

    double uptime;
    if (fscanf(uptime_file, "%lf", &uptime) != 1) {
        fclose(uptime_file);
        return 0;
    }

    fclose(uptime_file);
    return uptime;
}
#endif