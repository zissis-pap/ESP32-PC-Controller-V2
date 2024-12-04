#include <max7219.h>

#define RUN_OK      0
#define RUN_ERROR   1

#define DISPLAY_BUSY            -1
#define NO_TITLES_AVAILABLE      0
#define TITLES_REMAIN            1


char * receive_news(void);
int ScrollNews(max7219_t *dev, char *text, bool *display_available, uint8_t *display_user);
uint8_t parse_news_response(char *response);
void replace_utf8_apostrophes(char *text);