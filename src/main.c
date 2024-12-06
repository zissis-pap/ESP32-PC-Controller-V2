#include <stdio.h>
#include <main.h>
#include <definitions.h>
#include <tasks.h>

void app_main(void)
{
    max7219_t dev;
    gpio_config_c gpio_pins;
    QueueHandle_t xDataQueue = xQueueCreate(MESSAGE_QUEUE_LENGTH, MESSAGE_QUEUE_ITEM_SIZE * sizeof(char));
    QueueHandle_t xCommandQueue = xQueueCreate(COMMAND_QUEUE_LENGTH, COMMAND_QUEUE_ITEM_SIZE * sizeof(char));
    queue_struct_c queues = {.xDataQueue = xDataQueue, .xCommandQueue = xCommandQueue};
    setup_parameters_c SetupStruct = {.dev = &dev, .gpios = &gpio_pins};
    sys_disp_params_c DisplayConfig = {.dev = &dev, .display_available = true};
    disp_task_params_c taskParams = {.xQueue = xDataQueue, .displayParams = &DisplayConfig};
    command_handler_params_c commandParams = {.queues = &queues, .pins = &gpio_pins};
    
    SetupSystem(&SetupStruct);
    InitAction(&dev);

    xTaskCreate(PowerController, "power_controller", 2048, &commandParams, 15, NULL);
    xTaskCreate(CommandHandler, "command_handler", 2048, &commandParams, 12, NULL);
    xTaskCreate(DotMatrixDisplayTime, "display_time", 2048, &DisplayConfig, 8, NULL);
    xTaskCreate(DotMatrixDisplayNews, "display_news", 40960, &DisplayConfig, 10, NULL);
    xTaskCreate(TelegramPollUpdates, "poll_updates", 16384, &queues, 5, NULL);
    xTaskCreate(DotMatrixDisplayMessages, "display_telegram", 2048, &taskParams, 14, NULL);
    
    while(1)
    {   
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    esp_restart(); // If main task exits, restart the system
}
