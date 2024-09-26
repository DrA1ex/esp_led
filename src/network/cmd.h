#pragma once

#include <cstdint>

#include "lib/utils/enum.h"

MAKE_ENUM(PacketType, uint8_t,
    POWER, 0x01,
    BRIGHTNESS, 0x02,

    COLOR, 0x10,
    CALIBRATION, 0x11,
    TEMPERATURE, 0x12,

    NIGHT_MODE_ENABLED, 0x20,
    NIGHT_MODE_START, 0x21,
    NIGHT_MODE_END, 0x22,
    NIGHT_MODE_INTERVAL, 0x23,
    NIGHT_MODE_BRIGHTNESS, 0x24,

    SYS_CONFIG_MDNS_NAME, 0x60,

    SYS_CONFIG_WIFI_MODE, 0x61,
    SYS_CONFIG_WIFI_SSID, 0x62,
    SYS_CONFIG_WIFI_PASSWORD, 0x63,
    SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL, 0x64,
    SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL, 0x65,

    SYS_LED_TYPE, 0x66,
    SYS_CONFIG_LED_R_PIN, 0x67,
    SYS_CONFIG_LED_G_PIN, 0x68,
    SYS_CONFIG_LED_B_PIN, 0x69,

    SYS_CONFIG_POWER_CHANGE_TIMEOUT, 0x6A,
    SYS_CONFIG_WIFI_CONNECT_FLASH_TIMEOUT, 0x6B,

    SYS_CONFIG_TIME_ZONE, 0x6C,

    SYS_CONFIG_WEB_AUTH_ENABLED, 0x6D,
    SYS_CONFIG_WEB_AUTH_USER, 0x6E,
    SYS_CONFIG_WEB_AUTH_PASSWORD, 0x6F,

    SYS_CONFIG_MQTT_ENABLED, 0x70,
    SYS_CONFIG_MQTT_HOST, 0x71,
    SYS_CONFIG_MQTT_PORT, 0x72,
    SYS_CONFIG_MQTT_USER, 0x73,
    SYS_CONFIG_MQTT_PASSWORD, 0x74,
    SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS, 0x75,

    SYS_CONFIG_LED_MIN_BRIGHTNESS, 0x76,
    SYS_CONFIG_LED_MIN_TEMPERATURE, 0x77,
    SYS_CONFIG_LED_MAX_TEMPERATURE, 0x78,

    GET_CONFIG, 0xa0,
    RESTART, 0xb0,
)
