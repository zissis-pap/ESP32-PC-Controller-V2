#include <main.h>
#include <string.h>
#include <time_keep.h>
#include <telegram.h>
#include <display_user.h>
#include <fetch_news.h>
#include <tasks.h>
#include <definitions.h>

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayTelegramMessages(void *pvParameters)
{
    task_parameters_c *receivedParams = (task_parameters_c*) pvParameters;
    char receivedMessage[QUEUE_ITEM_SIZE] = "";
    while(1)
    {
        if (xQueueReceive(receivedParams->xQueue, receivedMessage, portMAX_DELAY) == pdPASS) 
        {
            do
            {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            while (!ScrollTelegramMessage(\
                receivedParams->displayParams->dev,\
                receivedMessage, &(receivedParams->displayParams->display_available),\
                &(receivedParams->displayParams->display_user)));
        }
        UBaseType_t remainingMessages = uxQueueMessagesWaiting(receivedParams->xQueue);
        if(remainingMessages == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayTime(void *pvParameters)
{
    display_parameters_c *receivedParams = (display_parameters_c*) pvParameters;
    while(1)
    {
        displayTime(receivedParams->dev, &(receivedParams->display_available), &(receivedParams->display_user));
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayNews(void *pvParameters)
{
    display_parameters_c *receivedParams = (display_parameters_c*) pvParameters;
    char *data = NULL;
    int remaining = NO_TITLES_AVAILABLE;
    while(1)
    {
        if(remaining == NO_TITLES_AVAILABLE) 
        {
            if(data != NULL) 
            {
                free(data);
            }
            data = receive_news();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        if(data != NULL)
        {
            remaining = ScrollNews(receivedParams->dev, data, &(receivedParams->display_available), &(receivedParams->display_user));
            vTaskDelay(pdMS_TO_TICKS(60000));
        }
    }
}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void TelegramPollUpdates(void *pvParameters)
{
    QueueHandle_t xQueue = (QueueHandle_t)pvParameters; // Cast parameter to the correct type
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

