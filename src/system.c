#include <nvs_flash.h>
#include <system.h>
#include <wifi.h>
#include <time_keep.h>
#include <telegram.h>
#include <esp_netif.h>
#include <string.h>

#define ESP_INTR_FLAG_DEFAULT 0

/**
 * @brief 
 * 
 * @param arg 
 */
static void IRAM_ATTR gpio_isr_handler(void *arg) 
{
    volatile bool *flag = (volatile bool *)arg;
    *flag = true; // Set the interrupt flag
}

/**
 * @brief 
 * 
 */
void ErrorHandler(void)
{

}

/**
 * @brief 
 * 
 * @param sp 
 */
void SetupSystem(setup_parameters_c *sp)
{
    SetupSPIbus();
    SetupNVS();
    SetupGPIOPins(sp->gpios);
    SetupDotMatrixDisplay(sp->dev);
    max7219_print_static_text(sp->dev, "Hello!");
    wifi_init();       // Initialize Wi-Fi

    initializeSNTP();  // Initialize SNTP
}

/**
 * @brief 
 * 
 */
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

/**
 * @brief 
 * 
 * @param dev 
 */
void SetupDotMatrixDisplay(max7219_t *dev)
{
    dev->cascade_size = CASCADE_SIZE;
    dev->digits = 0;
    dev->mirrored = true;
    dev->bcd = true;

    ESP_ERROR_CHECK(max7219_init_desc(dev, HOST, MAX7219_MAX_CLOCK_SPEED_HZ, CS_PIN));
    ESP_ERROR_CHECK(max7219_init(dev));
    max7219_set_brightness(dev, 0);
}

/**
 * @brief 
 * 
 */
void SetupSPIbus(void)
{
    spi_bus_config_t cfg = 
    {
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

/**
 * @brief 
 * 
 * @param gpio_pins 
 */
void SetupGPIOPins(gpio_config_c *gpio_pins)
{
    gpio_pins->input_1_gpio = GPIO_NUM_0;   // Set input 1 GPIO pin
    gpio_pins->input_2_gpio = GPIO_NUM_1;   // Set input 2 GPIO pin
    gpio_pins->input_3_gpio = GPIO_NUM_4;   // Set input 3 GPIO pin
    gpio_pins->output_1_gpio = GPIO_NUM_2;  // Set output 1 GPIO pin
    gpio_pins->output_2_gpio = GPIO_NUM_3;  // Set output 2 GPIO pin
    gpio_pins->interrupt_gpio_1 = false;    // Initialize the interrupt flags
    gpio_pins->interrupt_gpio_2 = false;
    gpio_pins->interrupt_gpio_3 = false;

    // Configure input GPIOs
    gpio_config_t io_conf_input = 
    {
        .pin_bit_mask = (1ULL << gpio_pins->input_1_gpio) |\
                        (1ULL << gpio_pins->input_2_gpio) |\
                        (1ULL << gpio_pins->input_3_gpio),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    gpio_config(&io_conf_input);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // Attach interrupt handler
    gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void *) &gpio_pins->interrupt_gpio_1);
    gpio_isr_handler_add(GPIO_NUM_1, gpio_isr_handler, (void *) &gpio_pins->interrupt_gpio_2);
    gpio_isr_handler_add(GPIO_NUM_4, gpio_isr_handler, (void *) &gpio_pins->interrupt_gpio_3);
    // Configure output GPIOs
    gpio_config_t io_conf_output = 
    {
        .pin_bit_mask = (1ULL << gpio_pins->output_1_gpio) | (1ULL << gpio_pins->output_2_gpio),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_output);

    gpio_set_level(gpio_pins->output_1_gpio, 1); // Set initial output level to high (Active low relay)
    gpio_set_level(gpio_pins->output_2_gpio, 1); // Set initial output level to high (Active low relay)
}

/**
 * @brief 
 * 
 * @param dev 
 */
void InitAction(max7219_t *dev)
{
    char data[64] = "";
    const char data_format[64] = "MAXIMUS-III+power+restored!%%0AAssigned+IP+is:+%d.%d.%d.%d";
    esp_netif_ip_info_t ip_info;
    vTaskDelay(pdMS_TO_TICKS(4000));
    // max7219_scroll_text(dev, "PC Controller V2.0", 100);
    esp_err_t esp_netif_get_ip_info(esp_netif_t *esp_netif, esp_netif_ip_info_t *ip_info);
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK)
    {
        sprintf(data, data_format, IP2STR(&ip_info.ip));
    }
    else
    {
        strcpy(data, "MAXIMUS-III+power+restored!%%0AWiFi+Connection+Failure");
    }
    send_telegram_message(data);
}

/**
 * @brief 
 * 
 * @param pins 
 * @return true 
 * @return false 
 */
bool MonitorSystemPower(gpio_config_c *pins)
{
    return gpio_get_level(pins->input_3_gpio) == 1;
}

/**
 * @brief 
 * 
 * @param data 
 * @return int 
 */
int ExtractCommand(const char * data)
{
    const CommandMapping commands[] = 
    {
        {"/power_on",           POWER_ON_COMMAND},
        {"/power_off",          POWER_OFF_COMMAND},
        {"/reset",              RESET_COMMAND},
        {"/force_power_off",    FORCE_POWER_OFF_COMMAND},
    };
    for (int i = 0; i < 4; ++i) 
    {
        if (strstr(data, commands[i].command) != NULL) 
        {
            return commands[i].value;
        }
    }
    return 0; // Default case if no command matches
}

/**
 * @brief 
 * 
 * @param pins 
 * @param system_status 
 */
void PowerON_OFF(gpio_config_c *pins, int system_status)
{
    gpio_set_level(pins->output_1_gpio, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(pins->output_1_gpio, 1);
}

/**
 * @brief 
 * 
 * @param pins 
 * @param system_status 
 */
void Reset(gpio_config_c *pins, int system_status)
{
    if(system_status)
    {
        gpio_set_level(pins->output_2_gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(pins->output_2_gpio, 1);
    }
    else
    {

    }
}

/**
 * @brief 
 * 
 * @param pins 
 * @param system_status 
 */
void ForcePowerOFF(gpio_config_c *pins, int system_status)
{
    if(system_status)
    {
        gpio_set_level(pins->output_1_gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(5000));
        gpio_set_level(pins->output_1_gpio, 1);
    }
    else
    {

    }
}
