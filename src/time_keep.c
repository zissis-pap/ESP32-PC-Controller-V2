#include <esp_sntp.h>
#include <esp_log.h>
#include <time.h>
#include <time_keep.h>
#include <definitions.h>
#include <display_user.h>

static const char *TAG = "SNTP";

/**
 * @brief 
 * 
 */
void initializeSNTP(void) 
{
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);        // Set to poll mode
    esp_sntp_setservername(0, "gr.pool.ntp.org");       // Default NTP server
    sntp_set_sync_interval(10000);                      // Sync every 10 seconds
    setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);    // Set TZ environment variable
    tzset();                                            // Apply the timezone setting
    esp_sntp_init();                                    // Start SNTP
}

/**
 * @brief 
 * 
 * @param dev 
 * @param display_available 
 * @param display_user 
 */
void displayTime(max7219_t *dev, bool *display_available, uint8_t *display_user)
{
    static int last_minute = 0;
    if(!(*display_available)) 
    {
        return;
    }
    *display_available = false;
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    if(*display_user != DISPLAY_TIME || last_minute != timeinfo.tm_min)
    {
        if(timeinfo.tm_year > (2020 - 1900))
        {
            {
                char t_str[6] = "";
                sprintf(t_str, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
                max7219_print_static_text(dev, t_str);
            }
        }
        else
        {
            max7219_print_static_text(dev, "--:--");
        }
        last_minute = timeinfo.tm_min;
    }
    *display_user = DISPLAY_TIME;
    *display_available = true;
}

