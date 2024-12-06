#include <max7219.h>
#include <driver/gpio.h>
#include <definitions.h>

/**
 * @brief 
 * 
 */
typedef struct 
{
    max7219_t *dev;
    bool display_available;
    uint8_t display_user;
} sys_disp_params_c;

/**
 * @brief 
 * 
 */
typedef struct
{
    gpio_num_t input_1_gpio;        // GPIO pin for input
    gpio_num_t input_2_gpio;        // GPIO pin for input
    gpio_num_t input_3_gpio;        // GPIO pin for input
    gpio_num_t output_1_gpio;       // GPIO pin for output
    gpio_num_t output_2_gpio;       // GPIO pin for output
    volatile bool interrupt_gpio_1;
    volatile bool interrupt_gpio_2;
    volatile bool interrupt_gpio_3;
} gpio_config_c;

/**
 * @brief 
 * 
 */
typedef struct
{  
    max7219_t *dev;
    gpio_config_c *gpios;
   
} setup_parameters_c;

/**
 * @brief 
 * 
 */
typedef struct 
{
    const char *command;
    int value;
} CommandMapping;

/**
 * @brief 
 * 
 */
void ErrorHandler(void);

/**
 * @brief 
 * 
 * @param dp 
 */
void SetupSystem(setup_parameters_c *sp);

/**
 * @brief 
 * 
 */
void SetupSPIbus(void);

/**
 * @brief 
 * 
 */
void SetupNVS(void);

/**
 * @brief 
 * 
 * @param dev 
 */
void SetupDotMatrixDisplay(max7219_t *dev);

/**
 * @brief 
 * 
 */
void SetupSPIbus(void);

/**
 * @brief 
 * 
 * @param gpio_pins 
 */
void SetupGPIOPins(gpio_config_c *gpio_pins);

/**
 * @brief 
 * 
 * @param dev 
 */
void InitAction(max7219_t *dev);

/**
 * @brief 
 * 
 * @param pins 
 * @return true 
 * @return false 
 */
bool MonitorSystemPower(gpio_config_c *pins);

/**
 * @brief 
 * 
 * @param data 
 * @return int 
 */
int ExtractCommand(const char * data);

/**
 * @brief 
 * 
 * @param pins 
 * @param system_status 
 */
void PowerON_OFF(gpio_config_c *pins, int system_status);

/**
 * @brief 
 * 
 * @param pins 
 * @param system_status 
 */
void Reset(gpio_config_c *pins, int system_status);

/**
 * @brief 
 * 
 * @param pins 
 * @param system_status 
 */
void ForcePowerOFF(gpio_config_c *pins, int system_status);