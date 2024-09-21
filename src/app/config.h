#pragma once

#include <cstdint>
#include <lib/network/wifi.h>
#include <lib/utils/enum.h>

#include "credentials.h"
#include "constants.h"

MAKE_ENUM_AUTO(AppState, uint8_t,
               UNINITIALIZED,
               INITIALIZATION,
               STAND_BY,
               TURNING_ON,
               TURNING_OFF
);

typedef char ConfigString[CONFIG_STRING_SIZE];

struct __attribute ((packed)) SysConfig {
    ConfigString mdns_name{MDNS_NAME};

    WifiMode wifi_mode = WIFI_MODE;
    ConfigString wifi_ssid{WIFI_SSID};
    ConfigString wifi_password{WIFI_PASSWORD};

    uint32_t wifi_connection_check_interval = WIFI_CONNECTION_CHECK_INTERVAL;
    uint32_t wifi_max_connection_attempt_interval = WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL;

    bool rgb_mode = RGB_MODE_ENABLED;
    uint8_t led_r_pin = LED_R_PIN;
    uint8_t led_g_pin = LED_G_PIN;
    uint8_t led_b_pin = LED_B_PIN;

    uint16_t led_min_brightness = LED_MIN_BRIGHTNESS;
    uint32_t power_change_timeout = POWER_CHANGE_TIMEOUT;
    uint32_t wifi_connect_flash_timeout = WIFI_CONNECT_FLASH_TIMEOUT;

    float time_zone = TIME_ZONE;

    bool web_auth = WEB_AUTH;
    ConfigString web_auth_user = WEB_AUTH_USER;
    ConfigString web_auth_password = WEB_AUTH_PASSWORD;

    bool mqtt = MQTT;
    ConfigString mqtt_host = MQTT_HOST;
    uint16_t mqtt_port = MQTT_PORT;
    ConfigString mqtt_user = MQTT_USER;
    ConfigString mqtt_password = MQTT_PASSWORD;

    bool mqtt_convert_brightness = MQTT_CONVERT_BRIGHTNESS;
};

struct __attribute ((packed)) NightModeConfig {
    bool enabled = false;
    uint16_t brightness = 10;
    uint32_t start_time = 0;
    uint32_t end_time = 10 * 60 * 60; // 22:00
    uint16_t switch_interval = 15 * 60; // 15 minutes
};

struct __attribute ((packed)) Config {
    bool power = true;
    uint16_t brightness = 2048;

    uint32_t color = ~0u; // All colors
    uint32_t calibration = ~0u; // No calibration

    NightModeConfig night_mode{};

    SysConfig sys_config{};
};
