#include <max7219.h>

/**
 * @brief 
 * 
 */
void initializeSNTP(void);

/**
 * @brief 
 * 
 * @param dev 
 * @param display_available 
 * @param display_user 
 */
void displayTime(max7219_t *dev, bool *display_available, uint8_t *display_user);