#include <max7219.h>

void initializeSNTP(void);
void displayTime(max7219_t *dev, bool *display_available, uint8_t *display_user);