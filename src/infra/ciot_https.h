/**
 * @file ciot_https.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-10-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __CIOT_HTTPS__H__
#define __CIOT_HTTPS__H__

#include "ciot_https_data.h"
#include "ciot_iface.h"
#include "ciot_err.h"

typedef struct ciot_https *ciot_https_t;

typedef enum ciot_https_event_id
{
    CIOT_HTTPS_EVENT_DATA = CIOT_IFACE_EVENT_CUSTOM
} ciot_https_event_id_t;

ciot_https_t ciot_https_new(void *handle);

ciot_err_t ciot_https_start(ciot_https_t this, ciot_https_cfg_t *cfg);
ciot_err_t ciot_https_stop(ciot_https_t this);
ciot_err_t ciot_https_process_req(ciot_https_t this, ciot_https_req_t *req);
ciot_err_t ciot_https_send_data(ciot_https_t this, uint8_t *data, int size);

#endif  //!__CIOT_HTTPS__H__