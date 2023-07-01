/**
 * @file ciot_custom_config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_CUSTOM_CONFIG__H__
#define __CIOT_CUSTOM_CONFIG__H__

#if defined(IDF_VER)
#include "ciot_config_idf.h"
#elif defined(_WIN32)
#include "ciot_config_win.h"
#endif  //IDF_VER

#endif  //!__CIOT_CUSTOM_CONFIG__H__