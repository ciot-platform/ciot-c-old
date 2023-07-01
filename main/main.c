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
#include "ciot_config.h"

void app_main(void)
{
    ciot_app_config_t app = {
#if CIOT_CONFIG_FEATURE_WIFI
        .wifi = {
            .mode = CIOT_CONFIG_WIFI_AP_MODE,
            .ssid = CIOT_CONFIG_WIFI_AP_SSID,
            .password = CIOT_CONFIG_WIFI_AP_PASSWORD,
        },
#endif
        .http_server = {
            .port = CIOT_CONFIG_HTTP_SERVER_PORT,
            .endpoint = CIOT_CONFIG_HTTP_SERVER_ENDPOINT
        }
    };
    
    ciot_app_start(&app);

    // Implement your application specific logic here
}

#if defined(_WIN32) || defined(__linux__)

int	main(int argc, char **argv)
{
    app_main();
    while(true){
        // Implement your application specific logic here
    }
}

#endif  //_WIN32
