#define TELEGRAM_GET_UPDATES    0 
#define TELEGRAM_SEND_TEXT      1

typedef struct 
{
    uint8_t count;
    int request_id[5];
    char *text[5];
} TelegramResponse;

char *make_telegram_url(const char *message, uint8_t select);
void send_telegram_message(const char *message);
void get_telegram_updates(void);
void parse_telegram_response(char *response);