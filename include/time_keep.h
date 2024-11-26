#include <max7219.h>

void initializeSNTP(void);
// Get and print the current time
void ObtainNTPTime(void);
void displayTime(max7219_t *dev);