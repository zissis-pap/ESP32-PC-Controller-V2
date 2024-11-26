#ifndef MAIN_H
#define MAIN_H
#include <max7219.h>

typedef struct
{
    max7219_t *dev;
    char * text;
    uint16_t delay;
} TaskParams;

typedef struct SetupParams
{
    max7219_t *dev;
};

void SetupSPIbus(void);
void SetupDotMatrixDisplay(max7219_t *dev);

#endif