#include <esp_wifi.h>
#include <esp_http_client.h>
#include <esp_log.h>
#include <fetch_news.h>
#include <wifi.h>
#include <cJSON.h>
#include <keys.h>

static const char* TAG = "NEWS";
esp_http_client_handle_t client;

void parse_news_response(const char *response) 
{
    cJSON *json = cJSON_Parse(response);
    if (json == NULL) 
    {
        ESP_LOGE("JSON", "Failed to parse JSON response");
        return;
    }

    cJSON *articles = cJSON_GetObjectItem(json, "articles");
    cJSON *article = NULL;
    cJSON_ArrayForEach(article, articles) 
    {
        cJSON *title = cJSON_GetObjectItem(article, "title");
        cJSON *description = cJSON_GetObjectItem(article, "description");
        if (cJSON_IsString(title) && (title->valuestring != NULL)) 
        {
            ESP_LOGI("NEWS", "Title: %s", title->valuestring);
            if (cJSON_IsString(description) && (description->valuestring != NULL)) 
            {
                ESP_LOGI("NEWS", "Description: %s", description->valuestring);
            }
        }
    }

    cJSON_Delete(json);
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt) 
{
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
                // ESP_LOGI("HTTP_EVENT", "Response Body: %.*s", evt->data_len, (char*)evt->data);
                // parse_news_response((char*)evt->data);
                // printf("\n\nDATA LEN: %d\n", evt->data_len);
                // printf((char*)evt->data);
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

void receive_news(void) 
{
    if(getWiFiState())
    {
        const char *news_api = TELEGRAM_CHAT_ID;
        char url[512] = "";
        sprintf(url, "https://newsapi.org/v2/top-headlines?country=us&pageSize=1&apiKey=%s", news_api);

        esp_http_client_config_t config = 
        {
            .url = url,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .event_handler = _http_event_handler,
        };

        client = esp_http_client_init(&config);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_header(client, "Authorization", "Bearer Zissis");
        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK) 
        {
            ESP_LOGI(TAG, "News received successfully");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to receive news: %s", esp_err_to_name(err));
        }
        esp_http_client_cleanup(client);
        return;
    }
    ESP_LOGI(TAG, "WiFi is not connected\n");
}