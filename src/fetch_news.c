#include <esp_wifi.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <fetch_news.h>
#include <wifi.h>
#include <cJSON.h>
#include <keys.h>
#include <display_user.h>

#define NEWS_ARRAY_SIZE 32768
static const char* TAG = "NEWS";
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

char *receive_news(void) 
{
    char *responseBuffer;
    if(getWiFiState())
    {
        const char *news_api = NEWS_API_KEY;
        char url[256] = "";
        sprintf(url, "https://newsapi.org/v2/top-headlines?country=us&pageSize=20&apiKey=%s", news_api);
        
        responseBuffer = (char*)calloc(NEWS_ARRAY_SIZE, sizeof(char));
        esp_http_client_config_t config = 
        {
            .url = url,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (char*)responseBuffer,
            .event_handler = _http_event_handler,
        };

        client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_header(client, "Authorization", "Bearer Zissis");
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) 
        {
            ESP_LOGI(TAG, "News received successfully");
            replace_utf8_apostrophes(responseBuffer);
            parse_news_response(responseBuffer);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to receive news: %s", esp_err_to_name(err));
        }
        esp_http_client_cleanup(client);
        return responseBuffer;
    }
    ESP_LOGI(TAG, "WiFi is not connected\n");
    return NULL;
}

char* ScrollNews(max7219_t *dev, char *text, bool *display_available, uint8_t *display_user)
{
    if(!*(display_available)) return 0;
    *display_available = false;
    *display_user = DISPLAY_NEWS;
    const char delimiter[] = "\n";
    static char *token;
    
    if(token == NULL)
    {
        token = strtok(text, delimiter);
    }
    max7219_scroll_text(dev, token, 75);
    token = strtok(NULL, delimiter);
    *display_available = true;
    return token;
}

void parse_news_response(char *response) 
{
    cJSON *json = cJSON_Parse(response);
    if (json == NULL) 
    {
        ESP_LOGE("JSON", "Failed to parse JSON response");
        return;
    }
    memset(response, 0, NEWS_ARRAY_SIZE);
    cJSON *articles = cJSON_GetObjectItem(json, "articles");
    cJSON *article = NULL;
    cJSON_ArrayForEach(article, articles) 
    {
        cJSON *title = cJSON_GetObjectItem(article, "title");
        // cJSON *description = cJSON_GetObjectItem(article, "description");
        if (cJSON_IsString(title) && (title->valuestring != NULL)) 
        {
            strcat(response, title->valuestring);
            strcat(response, "\n");

            // if (cJSON_IsString(description) && (description->valuestring != NULL)) 
            // {
            //     ESP_LOGI("NEWS", "Description: %s", description->valuestring);
            // }
        }
    }
    cJSON_Delete(json);
}

void replace_utf8_apostrophes(char *text) 
{
    char *src = text;
    char *dest = text;

    while (*src) 
    {
        // Check for UTF-8 encoded apostrophe: 0xE2 0x80 0x99
        if ((unsigned char)src[0] == 0xE2 && (unsigned char)src[1] == 0x80 && (unsigned char)src[2] == 0x99) 
        {
            *dest++ = '\'';  // Replace with ASCII apostrophe
            src += 3;        // Skip the 3-byte UTF-8 sequence
        } 
        else 
        {
            *dest++ = *src++; // Copy other characters as-is
        }
    }
    *dest = '\0'; // Null-terminate the resulting string
}
