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
#include "mongoose.h"

#ifdef _WIN32
#include "windows.h"
#else
#include <pthread.h>
#endif

#ifdef _WIN32
#define ciot_thread_t HANDLE
#else
#define ciot_thread_t pthread_t
#endif

typedef struct ciot_app_data
{
    ciot_msg_t msg;
    bool msg_received;
    bool msg_processed;
    ciot_thread_t thread;
} ciot_app_data_t;

static ciot_app_data_t this;
struct mg_mgr mgr;

static void* ciot_app_task(void* args);
static ciot_err_t ciot_app_create_thread();

ciot_err_t ciot_app_start(ciot_app_config_t *conf)
{
    if (ciot_app_create_thread())
    {
        return CIOT_ERR_FAIL;
    }

    mg_mgr_init(&mgr);
    ciot_app_init(conf);

    while(true){
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

    return CIOT_ERR_OK;
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

static void* ciot_app_task(void* args)
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

static ciot_err_t ciot_app_create_thread()
{
#ifdef _WIN32
    this.thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ciot_app_task, NULL, 0, NULL);
    if (this.thread == NULL)
    {
        return CIOT_ERR_FAIL;
    }
#else
    int result = pthread_create(&this.thread, NULL, ciot_app_task, NULL);
    if (result != 0)
    {
        return CIOT_ERR_FAIL;
    }
#endif
    return CIOT_ERR_OK;
}