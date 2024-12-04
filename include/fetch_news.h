#include <max7219.h>

#define RUN_OK      0
#define RUN_ERROR   1

#define DISPLAY_BUSY            -1
#define NO_TITLES_AVAILABLE      0
#define TITLES_REMAIN            1

#define NEWS_ARRAY_SIZE          32768

/**
 * @brief 
 * 
 * @return char* 
 */
char * receive_news(void);

/**
 * @brief 
 * 
 * @param dev 
 * @param text 
 * @param display_available 
 * @param display_user 
 * @return int 
 */
int ScrollNews(max7219_t *dev, char *text, bool *display_available, uint8_t *display_user);

/**
 * @brief 
 * 
 * @param response 
 * @return uint8_t 
 */
uint8_t parse_news_response(char *response);

/**
 * @brief 
 * 
 * @param text 
 */
void replace_utf8_apostrophes(char *text);