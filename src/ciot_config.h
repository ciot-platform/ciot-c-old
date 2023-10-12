/**
 * @file ciot_config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_CONFIG__H__
#define __CIOT_CONFIG__H__

#define CIOT_CONFIG_FEATURE_HTTPS 1  ///!< Enable HTTP Server
#define CIOT_CONFIG_FEATURE_HTTPC 1  ///!< Enable HTTP Client
#define CIOT_CONFIG_FEATURE_MQTT 1   ///!< Enable MQTT Client
#define CIOT_CONFIG_MESSAGE_LEN 240  ///!< HDR N expected message size
// #define CIOT_CONFIG_MESSAGE_LEN sizeof(ciot_msg_u)

#endif  //!__CIOT_CONFIG__H__