#define TELEGRAM_GET_UPDATES    0 
#define TELEGRAM_SEND_TEXT      1

#define MAX_TELEGRAM_REQUESTS   3

typedef struct 
{
    uint8_t count;
    int request_id[MAX_TELEGRAM_REQUESTS];
    char *text[MAX_TELEGRAM_REQUESTS];
} TelegramResponse;

char *make_telegram_url(const char *message, uint8_t select, int offset);
void send_telegram_message(const char *message);
TelegramResponse *get_telegram_updates(uint8_t count);
void telegram_set_offset(int offset);
void parse_telegram_response(TelegramResponse *saved_data, char *response, uint8_t count);