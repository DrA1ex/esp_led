#pragma once

#include "sys_constants.h"

#define WIFI_MODE                               (WIFI_AP_MODE)

#define WEB_AUTH                                (1)                     // Use basic auth for non-local connections

#define WIFI_CONNECTION_CHECK_INTERVAL          (5000u)                 // Interval (ms) between Wi-Fi connection check
#define WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL    (120000u)               // Max time (ms) to wait for Wi-Fi connection before switch to AP mode
                                                                        // 0 - Newer switch to AP mode

#define MDNS_NAME                               "esp_led"

#define LED_MODE                                (LedType::RGB)

#if ARDUINO_ARCH_ESP32
#define LED_R_PIN                               (2u)
#define LED_G_PIN                               (1u)
#define LED_B_PIN                               (0u)
#else
#define LED_R_PIN                               (2u) // D4
#define LED_G_PIN                               (4u) // D2
#define LED_B_PIN                               (5u) // D1
#endif

#define LED_MIN_BRIGHTNESS                      (1u)

#define BUTTON_ENABLED                          (false)
#define BUTTON_HIGH_STATE                       (true)
#if ARDUINO_ARCH_ESP32
#define BUTTON_PIN                              (3u)
#else
#define BUTTON_PIN                              (13u)
#endif

#define POWER_CHANGE_TIMEOUT                    (1000u)                // Timeout for power change animation
#define WIFI_CONNECT_FLASH_TIMEOUT              (3000u)

#define TIME_ZONE                               (5.f)                   // GMT +5:00


#define MQTT                                    (0)                     // Enable MQTT server

#define MQTT_CONNECTION_TIMEOUT                 (15000u)                // Connection attempt timeout to MQTT server
#define MQTT_RECONNECT_TIMEOUT                  (5000u)                 // Time before new reconnection attempt to MQTT server

#define MQTT_CONVERT_BRIGHTNESS                 (0u)                    // Convert brightness from internal range to [0..100]

#define MQTT_PREFIX                             ""
#define MQTT_TOPIC_BRIGHTNESS                   MQTT_PREFIX "/brightness"
#define MQTT_TOPIC_POWER                        MQTT_PREFIX "/power"
#define MQTT_TOPIC_COLOR                        MQTT_PREFIX "/color"
#define MQTT_TOPIC_TEMPERATURE                  MQTT_PREFIX "/temperature"
#define MQTT_TOPIC_NIGHT_MODE                   MQTT_PREFIX "/night_mode"

#define MQTT_OUT_PREFIX                         MQTT_PREFIX "/out"
#define MQTT_OUT_TOPIC_BRIGHTNESS               MQTT_OUT_PREFIX "/brightness"
#define MQTT_OUT_TOPIC_POWER                    MQTT_OUT_PREFIX "/power"
#define MQTT_OUT_TOPIC_COLOR                    MQTT_OUT_PREFIX "/color"
#define MQTT_OUT_TOPIC_TEMPERATURE              MQTT_OUT_PREFIX "/temperature"
#define MQTT_OUT_TOPIC_NIGHT_MODE               MQTT_OUT_PREFIX "/night_mode"
