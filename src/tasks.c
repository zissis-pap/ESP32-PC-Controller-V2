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
void PowerController(void *pvParameters)
{
    gpio_config_c *pins = (gpio_config_c*) pvParameters;
    while(1)
    {
        if(pins->interrupt_gpio_1)
        {
            pins->interrupt_gpio_1 = false;
        }
        if(pins->interrupt_gpio_2)
        {
            pins->interrupt_gpio_2 = false;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void CommandHandler(void *pvParameters)
{
    queue_struct_c *queues = (queue_struct_c*)pvParameters; // Cast parameter to the correct type

}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayTelegramMessages(void *pvParameters)
{
    task_parameters_c *receivedParams = (task_parameters_c*) pvParameters;
    char receivedMessage[MESSAGE_QUEUE_ITEM_SIZE] = "";
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
    queue_struct_c *queues = (queue_struct_c*)pvParameters; // Cast parameter to the correct type
    telegram_set_offset(-1); // Acknowledge all messages when rebooting
    char message[MESSAGE_QUEUE_ITEM_SIZE] = "";
    while(1)
    {
        TelegramResponse *data = get_telegram_updates(MAX_TELEGRAM_REQUESTS);
        uint8_t count = data->count;
        int offset = data->request_id[count-1] + 1;

        for(uint8_t i = 0; i < count; i++)
        {
            strcpy(message, data->text[i]);
            if (xQueueSend(queues->xDataQueue, message, pdMS_TO_TICKS(100)) == pdPASS) 
            {
                
            } 
            int command = ExtractCommand(data->text[i]);
            xQueueSend(queues->xCommandQueue, &command, pdMS_TO_TICKS(100));
            free(data->text[i]);
        }
        free(data);
        if(count != 0 ) telegram_set_offset(offset);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

