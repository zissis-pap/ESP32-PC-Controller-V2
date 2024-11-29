#ifndef MAIN_H
#define MAIN_H
#include <max7219.h>

typedef struct 
{
    max7219_t *dev;
    bool display_available;
    uint8_t display_user;
} DisplayParams;

void SetupSPIbus(void);
void SetupDotMatrixDisplay(max7219_t *dev);

#endif