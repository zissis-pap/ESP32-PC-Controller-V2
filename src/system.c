#include <main.h>
#include <definitions.h>
#include <max7219.h>
#include <nvs_flash.h>
#include <system.h>
#include <wifi.h>
#include <time_keep.h>
#include <telegram.h>


void SetupSystem(DisplayParams* dp)
{
    SetupSPIbus();
    SetupNVS();
    SetupDotMatrixDisplay(dp->dev);
    max7219_print_static_text(dp->dev, "Hello!");
    wifi_init();       // Initialize Wi-Fi
    initializeSNTP();  // Initialize SNTP
}

void SetupNVS(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
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

void ErrorHandler(void)
{

}

void InitAction(max7219_t *dev)
{
    // vTaskDelay(pdMS_TO_TICKS(4000));
    max7219_scroll_text(dev, "PC Controller V2.0", 100);
    send_telegram_message("MAXIMUS-III+power+restored!");
}
