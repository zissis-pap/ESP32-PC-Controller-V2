#include <max7219.h>

/**
 * @brief 
 * 
 */
typedef struct 
{
    max7219_t *dev;
    bool display_available;
    uint8_t display_user;
} DisplayParams;

/**
 * @brief 
 * 
 * @param dp 
 */
void SetupSystem(DisplayParams* dp);

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
 * @param dev 
 */
void InitAction(max7219_t *dev);