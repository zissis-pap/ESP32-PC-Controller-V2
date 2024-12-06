#include <stdio.h>
#include <main.h>
#include <definitions.h>
#include <tasks.h>

void app_main(void)
{
    max7219_t dev;
    gpio_config_c gpio_pins;
    QueueHandle_t xQueue = xQueueCreate(MESSAGE_QUEUE_LENGTH, MESSAGE_QUEUE_ITEM_SIZE * sizeof(char));
    QueueHandle_t xCommandQueue = xQueueCreate(COMMAND_QUEUE_LENGTH, COMMAND_QUEUE_ITEM_SIZE * sizeof(char));
    queue_struct_c queues = {.xDataQueue = xQueue, .xCommandQueue = xCommandQueue};
    setup_parameters_c SetupStruct = {.dev = &dev, .gpios = &gpio_pins};
    display_parameters_c DisplayConfig = {.dev = &dev, .display_available = true};
    task_parameters_c taskParams = {.xQueue = xQueue, .displayParams = &DisplayConfig};
    SetupSystem(&SetupStruct);
    InitAction(&dev);

    xTaskCreate(PowerController, "power_controller", 2048, &gpio_pins, 15, NULL);
    xTaskCreate(CommandHandler, "command_handler", 2048, &queues, 14, NULL);
    xTaskCreate(DotMatrixDisplayTime, "display_time", 2048, &DisplayConfig, 8, NULL);
    xTaskCreate(DotMatrixDisplayNews, "display_news", 40960, &DisplayConfig, 10, NULL);
    xTaskCreate(TelegramPollUpdates, "poll_updates", 16384, &queues, 5, NULL);
    xTaskCreate(DotMatrixDisplayTelegramMessages, "display_telegram", 2048, &taskParams, 12, NULL);
    
    while(1)
    {   
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    esp_restart(); // If main task exits, restart the system
}
