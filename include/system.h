#include "main.h"
#include "definitions.h"
#include "max7219.h"

void SetupSystem(struct SetupParams* sp);
void SetupDotMatrixDisplay(max7219_t *dev);
void SetupSPIbus(void);
void InitAction(max7219_t *dev);