#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <main.h>
#include <max7219.h>
#include <definitions.h>
#include <system.h>
#include <string.h>
#include <time_keep.h>
#include <telegram.h>
#include <display_user.h>
#include <fetch_news.h>

QueueHandle_t xQueue;

void DotMatrixDisplayTelegramMessages(void *pvParameters)
{
    DisplayParams *receivedParams = (DisplayParams*) pvParameters;
    char receivedMessage[QUEUE_ITEM_SIZE] = "";
    while(1)
    {
        if (xQueueReceive(xQueue, receivedMessage, portMAX_DELAY) == pdPASS) 
        {
            do
            {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            while (!ScrollTelegramMessage(receivedParams->dev, receivedMessage, &(receivedParams->display_available), &(receivedParams->display_user)));
        }
        UBaseType_t remainingMessages = uxQueueMessagesWaiting(xQueue);
        if(remainingMessages == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void DotMatrixDisplayTime(void *pvParameters)
{
    DisplayParams *receivedParams = (DisplayParams*) pvParameters;
    while(1)
    {
        displayTime(receivedParams->dev, &(receivedParams->display_available), &(receivedParams->display_user));
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void DotMatrixDisplayNews(void *pvParameters)
{
    DisplayParams *receivedParams = (DisplayParams*) pvParameters;
    char *data = NULL;
    char *remaining = NULL;
    while(1)
    {
        if(remaining == NULL) 
        {
            if(data != NULL) 
            {
                free(data);
            }
            data = receive_news();
        }
        if(data != NULL)
        {
            remaining = ScrollNews(receivedParams->dev, data, &(receivedParams->display_available), &(receivedParams->display_user));
        }
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}

void TelegramPollUpdates(void *pvParameters)
{
    telegram_set_offset(-1); // Acknowledge all messages when rebooting
    char message[QUEUE_ITEM_SIZE] = "";
    while(1)
    {
        TelegramResponse *data = get_telegram_updates(MAX_TELEGRAM_REQUESTS);
        uint8_t count = data->count;
        int offset = data->request_id[count-1] + 1;
        // printf("Count: %d\nOffset: %d\nID1: %d\nID2: %d\nID3: %d\n", count, offset, data->request_id[0], data->request_id[1], data->request_id[2]);
        // for(uint8_t i = 0; i < count; i++)
        // {
        //     printf(data->text[i]);
        //     printf("\n");
        // }
        for(uint8_t i = 0; i < count; i++)
        {
            strcpy(message, data->text[i]);
            if (xQueueSend(xQueue, message, pdMS_TO_TICKS(100)) == pdPASS) 
            {
                printf("Message added to Queue: %s\n", message);
            } 
            else 
            {
                printf("Failed to add message to Queue: %s\n", message);
            }
            free(data->text[i]);
        }
        free(data);
        if(count != 0 ) telegram_set_offset(offset);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE*sizeof(char));
    max7219_t dev;
    DisplayParams DisplayConfig = {.dev = &dev, .display_available = true};
    SetupSystem(&DisplayConfig);
    InitAction(&dev);

    // xTaskCreate(MasterTask, "master_task", 32768, &DisplayConfig, 5, NULL);
    xTaskCreate(DotMatrixDisplayTime, "display_time", 2048, &DisplayConfig, 8, NULL);
    xTaskCreate(DotMatrixDisplayNews, "display_news", 32768, &DisplayConfig, 10, NULL);
    xTaskCreate(TelegramPollUpdates, "poll_updates", 16384, NULL, 5, NULL);
    xTaskCreate(DotMatrixDisplayTelegramMessages, "display_telegram", 2048, &DisplayConfig, 12, NULL);
    
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
    esp_restart(); // If main task exits, restart the system
}
