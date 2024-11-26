// Need to sync time once per day

#include <esp_sntp.h>
#include <esp_log.h>
#include <time.h>
#include <time_keep.h>

#include <definitions.h>

extern const char *TAG;

void initializeSNTP(void) 
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);    // Set to poll mode
    esp_sntp_setservername(0, "gr.pool.ntp.org");   // Default NTP server
    sntp_set_sync_interval(10000);                  // Sync every 10 seconds
    esp_sntp_init();                                // Start SNTP
}

// Get and print the current time
void ObtainNTPTime(void) 
{
    setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);    // Set TZ environment variable
    tzset();                                            // Apply the timezone setting
    for(uint8_t retry = 0; retry < 10; retry++)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/10)", retry);
        if(sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retry >= 9) return; 
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void displayTime(max7219_t *dev)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    if(timeinfo.tm_year > (2020 - 1900))
    {
        char t_str[6] = "";
        sprintf(t_str, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        max7219_print_static_text(dev, t_str);
    }
    else
    {
        max7219_print_static_text(dev, "--:--");
    }
}

