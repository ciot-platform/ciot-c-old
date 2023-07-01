/**
 * @file ciot_app.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ciot_app.h"
#include "windows.h"

typedef struct ciot_app_data
{
    ciot_msg_t msg;
    bool msg_received;
    bool msg_processed;
    HANDLE thread;
} ciot_app_data_t;

static ciot_app_data_t this;

static DWORD WINAPI ciot_app_task(LPVOID args)
{
    printf("CIOT Application Start\n");

    for(;;)
    {
        if(this.msg_received)
        {
            ciot_err_t err = ciot_app_msg_handle(&this.msg);
            this.msg_received = false;
            this.msg_processed = true;
            printf("Message %d processed with err code %x\n", this.msg.type, err);
        }
    }
}

ciot_err_t ciot_app_start(ciot_app_config_t *conf)
{
    this.thread = CreateThread(NULL, 0, &ciot_app_task, NULL, 0, NULL);
    if (this.thread == NULL)
    {
        return CIOT_ERR_FAIL;
    }
    return ciot_app_init(conf);
}

ciot_err_t ciot_app_send_msg(ciot_msg_t *msg)
{
    if(this.msg_received)
    {
        return CIOT_ERR_BUSY;
    }
    else
    {
        this.msg = *msg;
        this.msg_received = true;
        return CIOT_ERR_OK;
    }
}

ciot_err_t ciot_app_wait_process(uint32_t timeout)
{
    time_t start_time = time(0);
    while (this.msg_processed == false)
    {
        time_t current_time = time(0);
        if((start_time + timeout) < current_time)
        {
            return CIOT_ERR_TIMEOUT;
        }
    }
    this.msg_processed = false;
    return CIOT_ERR_OK;
}
