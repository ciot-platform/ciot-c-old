/**
 * @file ciot_app.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ciot_app.h"
#include "ciot_tasks.h"
#include "ciot_storage.h"

#define CIOT_APP_EVENT_BIT_DONE BIT0

typedef struct ciot_app_data
{
    QueueHandle_t queue;
    EventGroupHandle_t event_group;
} ciot_app_data_t;

static ciot_app_data_t this;
static const char *TAG = "ciot_app";

static void ciot_app_task(void *args);

ciot_err_t ciot_app_start(ciot_app_config_t *conf)
{
    this.queue = xQueueCreate(1, sizeof(ciot_msg_t));
    this.event_group = xEventGroupCreate();
    xEventGroupSetBits(this.event_group, CIOT_APP_EVENT_BIT_DONE);

    xTaskCreatePinnedToCore(
        ciot_app_task,
        "ciot_app_task",
        CIOT_APP_TASK_STACK_SIZE,
        NULL,
        CIOT_APP_TASK_PRIORITY,
        NULL,
        CIOT_APP_TASK_CORE_ID);

    return ciot_app_init(conf);
}

ciot_err_t ciot_app_send_msg(ciot_msg_t *msg)
{
    if((xEventGroupGetBits(this.event_group) & CIOT_APP_EVENT_BIT_DONE) == true)
    {
        xEventGroupClearBits(this.event_group, CIOT_APP_EVENT_BIT_DONE);
        if (xQueueSend(this.queue, msg, portMAX_DELAY) == pdTRUE)
        {
            return CIOT_ERR_OK;
        }
        else
        {
            return CIOT_ERR_FAIL;
        }
    }
    else
    {
        return CIOT_ERR_BUSY;
    }
}

ciot_err_t ciot_app_wait_process(uint32_t timeout)
{
    xEventGroupWaitBits(this.event_group, CIOT_APP_EVENT_BIT_DONE, pdFALSE, pdFALSE, timeout / portTICK_PERIOD_MS);
    return CIOT_ERR_OK;
}

static void ciot_app_task(void *args)
{
    ESP_LOGI(TAG, "CIOT Application Start");

    ciot_msg_t *msg = malloc(sizeof(ciot_msg_t));

    for (;;)
    {
        if (xQueueReceive(this.queue, msg, portMAX_DELAY))
        {
            ciot_err_t err = ciot_app_msg_handle(msg);
            xEventGroupSetBits(this.event_group, CIOT_APP_EVENT_BIT_DONE);
            if (err != CIOT_ERR_OK)
            {
                ESP_LOGE(TAG, "Message %d processed with err code %x", msg->type, err);
            }
            else
            {
                ESP_LOGI(TAG, "Message %d processed with err code %x", msg->type, err);
            }
        }
    }
}
