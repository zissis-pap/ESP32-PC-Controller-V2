#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <main.h>
#include <max7219.h>
#include <definitions.h>
#include <system.h>
#include <string.h>
#include <time_keep.h>


void app_main()
{
    max7219_t dev;
    
    struct SetupParams SystemConfig = {.dev = &dev};
    
    SetupSystem(&SystemConfig);
    InitAction(&dev);


    while(1)
    {
        displayTime(&dev);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    esp_restart(); // If main task exits, restart the system
}
