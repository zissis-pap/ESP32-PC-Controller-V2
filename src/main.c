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


void ScrollText(max7219_t *dev, char *text, bool *display_available, uint8_t *display_user)
{
    if(!*(display_available)) return;

    *display_available = false;
    *display_user = MAIN_TASK;
    max7219_scroll_text(dev, text, 100);
    *display_available = true;
}

void MasterTask(void *pvParameters)
{
    DisplayParams *receivedParams = (DisplayParams*) pvParameters;
	while(1)
    {
        // ScrollText(receivedParams->dev, "Task 1 runs!", &(receivedParams->display_available), &(receivedParams->display_user));
        receive_news();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
    esp_restart(); // If main task exits, restart the system
}

void DotMatrixDisplayTime(void *pvParameters)
{
    DisplayParams *receivedParams = (DisplayParams*) pvParameters;
    while(1)
    {
        displayTime(receivedParams->dev, &(receivedParams->display_available), &(receivedParams->display_user));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void DotMatrixDisplayNews(void *pvParameters)
{
    DisplayParams *receivedParams = (DisplayParams*) pvParameters;
    uint8_t counter = 0;
    char *data = NULL;
    while(1)
    {
        if(counter == 0) 
        {
            if(data != NULL) free(data);
            data = receive_news();
            printf(data);
        }
        ScrollNews(receivedParams->dev, data, &(receivedParams->display_available), &(receivedParams->display_user));
        vTaskDelay(pdMS_TO_TICKS(5000));
        if(counter >= 19) counter = 0;
        else counter++;
    }
}

void app_main()
{
    max7219_t dev;
    
    DisplayParams DisplayConfig = {.dev = &dev, .display_available = true};
    SetupSystem(&DisplayConfig);
    InitAction(&dev);

    // xTaskCreate(MasterTask, "master_task", 32768, &DisplayConfig, 5, NULL);
    xTaskCreate(DotMatrixDisplayTime, "display_time", 2048, &DisplayConfig, 7, NULL);
    xTaskCreate(DotMatrixDisplayNews, "display_news", 32768, &DisplayConfig, 6, NULL);
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
    esp_restart(); // If main task exits, restart the system
}
