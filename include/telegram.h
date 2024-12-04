#include <max7219.h>

#define TELEGRAM_GET_UPDATES    0 
#define TELEGRAM_SEND_TEXT      1

#define MAX_TELEGRAM_REQUESTS   3

/**
 * @brief 
 * 
 */
typedef struct 
{
    uint8_t count;
    int request_id[MAX_TELEGRAM_REQUESTS];
    char *text[MAX_TELEGRAM_REQUESTS];
} TelegramResponse;

/**
 * @brief 
 * 
 * @param message 
 * @param select 
 * @param offset 
 * @return char* 
 */
char *make_telegram_url(const char *message, uint8_t select, int offset);

/**
 * @brief 
 * 
 * @param message 
 */
void send_telegram_message(const char *message);

/**
 * @brief Get the telegram updates object
 * 
 * @param count 
 * @return TelegramResponse* 
 */
TelegramResponse *get_telegram_updates(uint8_t count);

/**
 * @brief 
 * 
 * @param offset 
 */
void telegram_set_offset(int offset);

/**
 * @brief 
 * 
 * @param saved_data 
 * @param response 
 * @param count 
 */
void parse_telegram_response(TelegramResponse *saved_data, char *response, uint8_t count);

/**
 * @brief 
 * 
 * @param dev 
 * @param text 
 * @param display_available 
 * @param display_user 
 * @return uint8_t 
 */
uint8_t ScrollTelegramMessage(max7219_t *dev, char *text, bool *display_available, uint8_t *display_user);