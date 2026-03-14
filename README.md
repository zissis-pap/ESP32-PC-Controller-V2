# ESP32-PC-Controller-V2

Control a desktop PC's power, reset, and force-off from a Telegram bot, using an ESP32-C3 microcontroller. Status and news headlines are shown on a MAX7219 dot-matrix display.

## Features

- **Remote control via Telegram**: `/power_on`, `/power_off`, `/reset`, `/force_power_off`
- **Physical switch support**: Debounced GPIO inputs for power and reset buttons
- **PC power monitoring**: Detects and reports PC on/off state transitions
- **Dot-matrix display** (MAX7219): Scrolls time, news headlines, and event messages
- **News headlines**: Fetches top headlines from newsapi.org on startup

## Hardware

| Signal | GPIO |
|---|---|
| SPI MOSI | 7 |
| SPI CLK | 6 |
| SPI CS (MAX7219) | 10 |
| Power switch input | 0 |
| Reset switch input | 1 |
| PC power status input | 4 |
| Power relay output | 2 |
| Reset relay output | 3 |

Relay outputs are active-low.

## Setup

### 1. Clone the repository

```bash
git clone --recurse-submodules <repo-url>
```

### 2. Create your credentials file

Copy the template and fill in your values:

```bash
cp include/keys_template.h include/keys.h
```

Edit `include/keys.h`:

```c
#define WIFI_SSID           "your_wifi_ssid"
#define WIFI_PASS           "your_wifi_password"
#define TELEGRAM_BOT_TOKEN  "your_telegram_bot_token"
#define TELEGRAM_CHAT_ID    "your_telegram_chat_id"
#define NEWS_API_KEY        "your_newsapi_org_key"
```

`keys.h` is gitignored and must never be committed.

### 3. Configure sdkconfig

HTTPS requests are made without certificate verification. In `sdkconfig`:

- `CONFIG_ESP_TLS_INSECURE` → enable
- `CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY` → enable

### 4. Build and flash

```bash
pio run --target upload
```

## Telegram Commands

| Command | Action |
|---|---|
| `/power_on` | Press power button (only if PC is off) |
| `/power_off` | Press power button (only if PC is on) |
| `/reset` | Press reset button (only if PC is on) |
| `/force_power_off` | Hold power button 5 s (only if PC is on) |

Any other message is displayed on the dot-matrix display.

## Dependencies

- [ESP-IDF](https://github.com/espressif/esp-idf) via PlatformIO espressif32 platform
- [max7219](lib/max7219) — included as a git submodule
- [cJSON](https://github.com/DaveGamble/cJSON) — bundled with ESP-IDF
- [newsapi.org](https://newsapi.org) — free API key required
