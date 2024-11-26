#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_idf_version.h>
#include <main.h>
#include <max7219.h>
#include <string.h>
#include <defines.h>

void MasterTask(void *pvParameter)
{
	TaskParams *receivedParams = (TaskParams *)pvParameter;
	max7219_print_static_text(receivedParams->dev, receivedParams->text);
    max7219_scroll_text(receivedParams->dev, receivedParams->text, receivedParams->delay);
	while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    esp_restart(); // If main task exits, restart the system
}

void app_main()
{
    max7219_t dev;
    SetupSPIbus();
    SetupDotMatrixDisplay(&dev);
    // TaskParams display = {.dev = &dev, .text = "Hello World!", .delay = 100};
    // xTaskCreate(MasterTask, "Master", configMINIMAL_STACK_SIZE * 3, &display, 5, NULL);
    
    max7219_print_static_text(&dev, "Hello!");
    vTaskDelay(pdMS_TO_TICKS(2000));
    max7219_scroll_text(&dev, "PC Controller V2.0", 100);
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void SetupDotMatrixDisplay(max7219_t *dev)
{
    dev->cascade_size = CASCADE_SIZE;
    dev->digits = 0;
    dev->mirrored = true;
    dev->bcd = true;

    ESP_ERROR_CHECK(max7219_init_desc(dev, HOST, MAX7219_MAX_CLOCK_SPEED_HZ, CS_PIN));
    ESP_ERROR_CHECK(max7219_init(dev));
    max7219_set_brightness(dev, 1);
}

void SetupSPIbus(void)
{
    spi_bus_config_t cfg = {
       .mosi_io_num = MOSI_PIN,
       .miso_io_num = -1,
       .sclk_io_num = CLK_PIN,
       .quadwp_io_num = -1,
       .quadhd_io_num = -1,
       .max_transfer_sz = 0,
       .flags = 0
    };
    ESP_ERROR_CHECK(spi_bus_initialize(HOST, &cfg, SPI_DMA_CH_AUTO));
}