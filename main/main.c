/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h>
#include <stdio.h>

#include "ciot_app.h"

void app_main(void)
{
    ciot_app_config_t app = {
        .ap = {
            .ssid = "CIOT APP",
            .password = "administrator",
            .tcp.ip.dhcp = CIOT_TCP_DHCP_CONFIG_SERVER,
        },
        .http_server = {
            .port = 80,
            .endpoint = "/v1/ciot"
        }
    };
    ciot_app_start(&app);
}

#if defined(WIN32) || defined(__linux__)

int	main(int argc, char **argv)
{
    app_main();
    return 0;
}

#endif  //WIN32