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


void ScrollMessage(max7219_t *dev, char *text, bool *display_available, uint8_t *display_user)
{
    if(!*(display_available)) return;

    *display_available = false;
    *display_user = DISPLAY_MESSAGE;
    max7219_scroll_text(dev, text, 100);
    *display_available = true;
}

void DisplayTask(void *pvParameters)
{
    DisplayParams *receivedParams = (DisplayParams*) pvParameters;
    ScrollMessage(receivedParams->dev, "Hi!", &(receivedParams->display_available), &(receivedParams->display_user));
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
            printf(data->text[i]);
            free(data->text[i]);
        }
        free(data);
        if(count != 0 ) telegram_set_offset(offset);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main()
{
    max7219_t dev;
    
    DisplayParams DisplayConfig = {.dev = &dev, .display_available = true};
    SetupSystem(&DisplayConfig);
    InitAction(&dev);

    // xTaskCreate(MasterTask, "master_task", 32768, &DisplayConfig, 5, NULL);
    xTaskCreate(DotMatrixDisplayTime, "display_time", 2048, &DisplayConfig, 8, NULL);
    xTaskCreate(DotMatrixDisplayNews, "display_news", 32768, &DisplayConfig, 10, NULL);
    xTaskCreate(TelegramPollUpdates, "poll_updates", 16384, NULL, 5, NULL);
    
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
    esp_restart(); // If main task exits, restart the system
}
