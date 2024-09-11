#pragma once

#include <cstdint>
#include <lib/network/wifi.h>

#include "constants.h"
#include "credentials.h"

enum class AppState {
    UNINITIALIZED,
    INITIALIZATION,
    STAND_BY,
    TURNING_ON,
    TURNING_OFF
};

enum class ServiceState {
    UNINITIALIZED,
    WIFI_CONNECT,
    INITIALIZATION,
    STAND_BY
};

struct __attribute ((packed)) SysConfig {
    char mdns_name[16] {MDNS_NAME};

    WifiMode wifi_mode = WIFI_MODE;
    char wifi_ssid[32] {WIFI_SSID};
    char wifi_password[32] {WIFI_PASSWORD};

    uint32_t wifi_connection_check_interval = WIFI_CONNECTION_CHECK_INTERVAL;
    uint32_t wifi_max_connection_attempt_interval = WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL;

    bool rgb_mode = RGB_MODE_ENABLED;

    union {
        struct {
            uint8_t led_r_pin = LED_R_PIN;
            uint8_t led_g_pin = LED_G_PIN;
            uint8_t led_b_pin = LED_B_PIN;
        };

        struct {
            uint8_t led_pin;
        };
    };

    uint32_t power_change_timeout = POWER_CHANGE_TIMEOUT;
    uint32_t wifi_connect_flash_timeout = WIFI_CONNECT_FLASH_TIMEOUT;

    float time_zone = TIME_ZONE;

    bool web_auth = WEB_AUTH;
    char web_auth_user[16] = WEBAUTH_USER;
    char web_auth_password[16] = WEBAUTH_PASSWORD;

    bool mqtt = MQTT;
    char mqtt_host[32] = MQTT_HOST;
    uint16_t mqtt_port = MQTT_PORT;
    char mqtt_user[16] = MQTT_USER;
    char mqtt_password[16] = MQTT_PASSWORD;

    uint8_t mqtt_convert_brightness = MQTT_CONVERT_BRIGHTNESS;
};

struct __attribute ((packed)) NightModeConfig {
    bool enabled = false;

    uint16_t brightness = 10;

    uint32_t start_time = 0;
    uint32_t end_time = (uint32_t) 10 * 60 * 60;
    uint16_t switch_interval = (uint32_t) 15 * 60;
};

struct __attribute ((packed)) Config {
    bool power = true;

    uint16_t brightness = 2048;

    uint32_t color = ~0u;
    uint32_t calibration = ~0u;

    NightModeConfig night_mode {};

    SysConfig sys_config {};
};
