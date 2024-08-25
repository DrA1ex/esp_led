#pragma once

#include "sys_constants.h"

#define WIFI_MODE                               (WIFI_AP_MODE)

#define WEB_AUTH                                                        // Use basic auth for non-local connections

#define WIFI_CONNECTION_CHECK_INTERVAL          (5000u)                 // Interval (ms) between Wi-Fi connection check
#define WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL    (0u)                    // Max time (ms) to wait for Wi-Fi connection before switch to AP mode
                                                                        // 0 - Newer switch to AP mode

#define MDNS_NAME                               "esp_led"

#define LED_PIN                                 (1u)


#define POWER_CHANGE_TIMEOUT                    (1000u)                // Timeout for power change animation
#define WIFI_CONNECT_FLASH_TIMEOUT              (3000u)

#define TIME_ZONE                               (5.f)                   // GMT +5:00


//#define MQTT                                                          // Enable MQTT server

#define MQTT_CONNECTION_TIMEOUT                 (15000u)                // Connection attempt timeout to MQTT server
#define MQTT_RECONNECT_TIMEOUT                  (5000u)                 // Time before new reconnection attempt to MQTT server

#define MQTT_PREFIX                             MDNS_NAME
#define MQTT_TOPIC_BRIGHTNESS                   MQTT_PREFIX "/brightness"
#define MQTT_TOPIC_POWER                        MQTT_PREFIX "/power"

#define MQTT_OUT_PREFIX                         MQTT_PREFIX "/out"
#define MQTT_OUT_TOPIC_BRIGHTNESS               MQTT_OUT_PREFIX "/brightness"
#define MQTT_OUT_TOPIC_POWER                    MQTT_OUT_PREFIX "/power"
