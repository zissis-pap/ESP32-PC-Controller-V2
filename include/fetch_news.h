#include <max7219.h>

char * receive_news(void);
void ScrollNews(max7219_t *dev, char *text, bool *display_available, uint8_t *display_user);
void parse_news_response(char *response);
void replace_utf8_apostrophes(char *text);