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
    command_handler_params_c *params = (command_handler_params_c*) pvParameters;
    while(1)
    {
        if(params->pins->interrupt_gpio_1)
        {
            params->pins->interrupt_gpio_1 = false;
            gpio_intr_disable(params->pins->input_1_gpio);
            xQueueSend(params->queues->xDataQueue, "Power switch pressed", pdMS_TO_TICKS(100));
            PowerON(params->pins);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_intr_enable(params->pins->input_1_gpio);

        }
        if(params->pins->interrupt_gpio_2)
        {
            params->pins->interrupt_gpio_2 = false;
            gpio_intr_disable(params->pins->input_2_gpio);
            xQueueSend(params->queues->xDataQueue, "Reset switch pressed", pdMS_TO_TICKS(100));
            Reset(params->pins);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gpio_intr_enable(params->pins->input_2_gpio);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void CommandHandler(void *pvParameters)
{
    command_handler_params_c *params = (command_handler_params_c*)pvParameters; // Cast parameter to the correct type
    int command = 0;
    while(1)
    {
        if (xQueueReceive(params->queues->xCommandQueue, &command, portMAX_DELAY) == pdPASS) 
        {
            switch(command)
            {
                case POWER_ON_COMMAND:
                    xQueueSend(params->queues->xDataQueue, "Remote Power ON", pdMS_TO_TICKS(100));
                    PowerON(params->pins);
                    break;
                case POWER_OFF_COMMAND:
                    xQueueSend(params->queues->xDataQueue, "Remote Power OFF", pdMS_TO_TICKS(100));
                    PowerOFF(params->pins);
                    break;
                case RESET_COMMAND:
                    xQueueSend(params->queues->xDataQueue, "Remote Reset", pdMS_TO_TICKS(100));
                    Reset(params->pins);
                    break;
                case FORCE_POWER_OFF_COMMAND:
                    xQueueSend(params->queues->xDataQueue, "Remote Force Power OFF", pdMS_TO_TICKS(100));
                    ForcePowerOFF(params->pins);
                    break;
                default:
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief 
 * 
 * @param pvParameters 
 */
void DotMatrixDisplayMessages(void *pvParameters)
{
    disp_task_params_c *receivedParams = (disp_task_params_c*) pvParameters;
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
    sys_disp_params_c *receivedParams = (sys_disp_params_c*) pvParameters;
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
    sys_disp_params_c *receivedParams = (sys_disp_params_c*) pvParameters;
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
            
            int command = ExtractCommand(message);
            if(command)
            {
                xQueueSend(queues->xCommandQueue, &command, pdMS_TO_TICKS(100));
            }
            else
            {
                if (xQueueSend(queues->xDataQueue, message, pdMS_TO_TICKS(100)) == pdPASS) 
                {
                    
                } 
            }
            free(data->text[i]);
        }
        free(data);
        if(count != 0 ) telegram_set_offset(offset);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

