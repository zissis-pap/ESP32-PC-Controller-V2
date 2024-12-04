#include <max7219.h>

typedef struct 
{
    max7219_t *dev;
    bool display_available;
    uint8_t display_user;
} DisplayParams;

void SetupSystem(DisplayParams* dp);
void SetupSPIbus(void);
void SetupNVS(void);
void SetupDotMatrixDisplay(max7219_t *dev);
void SetupSPIbus(void);
void InitAction(max7219_t *dev);