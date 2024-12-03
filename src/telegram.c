#include <esp_http_client.h>
#include <esp_log.h>
#include <telegram.h>
#include <keys.h>
#include <wifi.h>
#include <string.h>
#include <cJSON.h>

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
        char *url = make_telegram_url(message, TELEGRAM_SEND_TEXT);

        esp_http_client_config_t config = 
        {
            .url = url,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
        };
        
        client = esp_http_client_init(&config);
        free(url);
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
        char *url = make_telegram_url(NULL, TELEGRAM_GET_UPDATES);

        esp_http_client_config_t config = 
        {
            .url = url,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (char*)responseBuffer,
            .event_handler = _http_event_handler,
        };

        client = esp_http_client_init(&config);
        free(url);
        esp_err_t err = esp_http_client_perform(client);
        
        if (err == ESP_OK) 
        {
            ESP_LOGI(TAG, "Request for updates sent successfully");
            parse_telegram_response(responseBuffer);
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

char *make_telegram_url(const char *message, uint8_t select)
{
    char *url;
    const char *telegram_url = "https://api.telegram.org/bot";
    const char *telegram_token = TELEGRAM_BOT_TOKEN;
    const char *telegram_chatid = TELEGRAM_CHAT_ID;
    url = (char*)malloc(512*sizeof(char));
    
    if(select == TELEGRAM_SEND_TEXT)
    {
        sprintf(url, "%s%s/sendMessage?chat_id=%s&text=%s", telegram_url, telegram_token, telegram_chatid, message);
    }
    else
    {
        sprintf(url, "%s%s/getUpdates", telegram_url, telegram_token);
    }
    return url;
}

void parse_telegram_response(char *response) 
{
    cJSON *json = cJSON_Parse(response);
    if (json == NULL) 
    {
        ESP_LOGE("JSON", "Failed to parse JSON response");
        return;
    }
    cJSON *result = cJSON_GetObjectItem(json, "result");
    if (result == NULL) 
    {
        printf("\"result\" key not found\n");
        cJSON_Delete(json);
        return;
    }
    int array_size = cJSON_GetArraySize(result);
    printf("Extracted data:\n");
    for (int i = 0; i < array_size; i++) 
    {
        cJSON *item = cJSON_GetArrayItem(result, i);
        if (item == NULL) continue;

        cJSON *update_id = cJSON_GetObjectItem(item, "update_id"); // Get "update_id"
        if (cJSON_IsNumber(update_id)) 
        {
            printf("update_id: %d\n", update_id->valueint);
        } 

        cJSON *message = cJSON_GetObjectItem(item, "message"); // Get "message" object
        if (message == NULL) 
        {
            continue;
        }

        cJSON *text = cJSON_GetObjectItem(message, "text"); // Get "text" from "message"
        if (cJSON_IsString(text)) 
        {
            printf("text: %s\n", text->valuestring);
        } 
    }
    cJSON_Delete(json);
}
