#pragma once

#include "sys_constants.h"

#define WIFI_MODE                               (WIFI_AP_MODE)
#define WIFI_SSID                               "ESP_LED"
#define WIFI_PASSWORD                           "12345678"

#define WIFI_CONNECTION_CHECK_INTERVAL          (5000u)                 // Interval (ms) between Wi-Fi connection check
#define WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL    (0u)                    // Max time (ms) to wait for Wi-Fi connection before switch to AP mode
                                                                        // 0 - Newer switch to AP mode

#define MDNS_NAME                               "esp_led"

#define LED_PIN                                 (1u)


#define POWER_CHANGE_TIMEOUT                    (1000u)                // Timeout for power change animation

#define TIME_ZONE                               (5.f)                   // GMT +5:00
