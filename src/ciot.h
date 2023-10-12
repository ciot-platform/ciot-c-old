/**
 * @file ciot_n.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT__H__
#define __CIOT__H__

#include "ciot_err.h"
#include "ciot_iface.h"

typedef struct ciot_n *ciot_t;

typedef enum ciot_state
{
    CIOT_STATE_IDLE,
    CIOT_STATE_BUSY,
    CIOT_STATE_ERROR,
} ciot_state_t; 

ciot_t ciot_new(void);
ciot_err_t ciot_set_iface_list(ciot_t this, ciot_iface_t *iface_list[], int count);

#endif  //!__CIOT__H__