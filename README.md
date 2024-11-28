# ESP32-PC-Controller-V2
This projects enables the user to control a desktop pc's power status from an ESP32 microcontroller

*Initially, this project makes https requests to telegram app without certificate. Therefore, please set in the sdkconfig: "Allow potentially insecure options" -> true\
"Skip server certificate verification by default" -> true\
Certificate implementation is planned for future releases*

## Instructions
* Download repository
* Create a **keys.h** file and put it in the **include** folder of the project
* In the **keys.h** file create 4 definitions like this:\
**#define WIFI_SSID** "YOUR-WIFI's-SSID"\
**#define WIFI_PASS** "YOUR-WIFI-s-PASSWORD "\
**#define TELEGRAM_BOT_TOKEN** "YOUR-TELEGRAM-BOT-TOKEN"\
**#define TELEGRAM_CHAT_ID** "YOUR-TELEGRAM-CHAT-ID"\