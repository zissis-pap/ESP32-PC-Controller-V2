#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <definitions.h>
#include <wifi.h>
#include <keys.h>

static const char *TAG = "WIFI";
static bool wifi_is_connected = false;

/**
 * @brief Event handler for Wi-Fi events
 * 
 * @param arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) 
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        setWiFiState(false);
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        setWiFiState(false);
        ESP_LOGI(TAG, "Retrying to connect to the Wi-Fi");
        esp_wifi_connect();
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        setWiFiState(true);
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

/**
 * @brief Initialize Wi-Fi
 * 
 */
void wifi_init(void) 
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &event_handler,
                                        NULL,
                                        &instance_any_id);

    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = 
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Wi-Fi initialization finished.");
}

/**
 * @brief Set the Wi Fi state object
 * 
 * @param state 
 */
void setWiFiState(bool state)
{
    wifi_is_connected = state;
}

/**
 * @brief Get the Wi Fi State object
 * 
 * @return true 
 * @return false 
 */
bool getWiFiState(void)
{
    return wifi_is_connected;
}

