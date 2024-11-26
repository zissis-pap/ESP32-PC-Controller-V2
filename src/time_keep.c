#include <esp_sntp.h>
#include <esp_log.h>
#include <time.h>
#include <time_keep.h>
#include <definitions.h>

extern const char *TAG;

void initialize_sntp(void) 
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL); // Set to poll mode
    esp_sntp_setservername(0, "gr.pool.ntp.org");  // Default NTP server
    esp_sntp_init(); // Start SNTP
}

// Get and print the current time
void obtain_time(void) 
{
    time_t now;
    struct tm timeinfo;
    int retry = 0;
    const int retry_count = 100;

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) 
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    time(&now);
    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year > (1970 - 1900)) 
    {
        ESP_LOGI(TAG, "Current time: %s", asctime(&timeinfo));
    } else {
        ESP_LOGI(TAG, "Failed to obtain time.");
    }
}
