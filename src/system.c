#include "system.h"

void SetupSystem(struct SetupParams* sp)
{
    SetupSPIbus();
    SetupDotMatrixDisplay(sp->dev);
}

void SetupDotMatrixDisplay(max7219_t *dev)
{
    dev->cascade_size = CASCADE_SIZE;
    dev->digits = 0;
    dev->mirrored = true;
    dev->bcd = true;

    ESP_ERROR_CHECK(max7219_init_desc(dev, HOST, MAX7219_MAX_CLOCK_SPEED_HZ, CS_PIN));
    ESP_ERROR_CHECK(max7219_init(dev));
    max7219_set_brightness(dev, 15);
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

void InitAction(max7219_t *dev)
{
    max7219_print_static_text(dev, "Hello!");
    vTaskDelay(pdMS_TO_TICKS(2000));
    max7219_scroll_text(dev, "PC Controller V2.0", 100);
}