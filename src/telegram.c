#include <esp_http_client.h>
#include <esp_log.h>
#include <telegram.h>
#include <keys.h>
#include <wifi.h>
#include <string.h>

static const char *TAG = "Telegram";
static esp_http_client_handle_t client;

static esp_err_t _http_event_handler(esp_http_client_event_t *evt) 
{
    static int output_len = 0;       // Stores number of bytes read
    switch (evt->event_id) 
    {
        case HTTP_EVENT_ERROR:
            ESP_LOGI("HTTP_EVENT", "Error occurred during HTTP request");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI("HTTP_EVENT", "Header Sent");
            break;
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(client)) 
            {
                memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                output_len += evt->data_len;
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            printf(evt->user_data);
            output_len = 0;
            break;
        default:
            break;
    }
    return ESP_OK;
}

void send_telegram_message(const char *message) 
{
    if(getWiFiState())
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

        client = esp_http_client_init(&config);
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

void get_telegram_updates(void)
{
    char *responseBuffer;
    if(getWiFiState())
    {
        responseBuffer = (char*)malloc(4096*sizeof(char));
        char url[128] = "";
        char *basic_url =  make_telegram_url();
        sprintf(url, "%s/getUpdates", basic_url);
        free(basic_url);

        esp_http_client_config_t config = 
        {
            .url = url,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (char*)responseBuffer,
            .event_handler = _http_event_handler,
        };

        client = esp_http_client_init(&config);
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) 
        {
            ESP_LOGI(TAG, "Request for updates sent successfully");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send message: %s", esp_err_to_name(err));
        }
        esp_http_client_cleanup(client);
        free(responseBuffer);
        return;
    }
}

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