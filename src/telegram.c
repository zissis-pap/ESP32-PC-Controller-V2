#include "esp_http_client.h"
#include "esp_log.h"
#include "telegram.h"
#include "keys.h"
#include "string.h"

static const char *TAG = "Telegram";
extern bool wifi_is_connected;

char *make_telegram_url(void)
{
    const char *telegram_url = "https://api.telegram.org/bot";
    const char *telegram_token = TELEGRAM_BOT_TOKEN;
    char *url;
    url = (char*)calloc(strlen(telegram_url) + strlen(telegram_token), sizeof(char));
    strcpy(url, telegram_url);
    strcat(url, telegram_token);
    return url;
}

void send_telegram_message(const char *message) 
{
    if(wifi_is_connected)
    {
        const char *telegram_chatid = TELEGRAM_CHAT_ID;
        char url[512] = "";
        char *basic_url =  make_telegram_url();
        sprintf(url, "%s/sendMessage?chat_id=%s&text=%s", basic_url, telegram_chatid, message);
        free(basic_url);

        esp_http_client_config_t config = 
        {
            .url = url,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) 
        {
            ESP_LOGI(TAG, "Message sent successfully");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send message: %s", esp_err_to_name(err));
        }
        esp_http_client_cleanup(client);
        return;
    }
    ESP_LOGI(TAG, "WiFi is not connected\n");
}