#include <max7219.h>

typedef struct
{
    max7219_t *dev;
    char * text;
    uint16_t delay;
} TaskParams;

void SetupSPIbus(void);
void SetupDotMatrixDisplay(max7219_t *dev);