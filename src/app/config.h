#pragma once

#include <cstdint>

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

struct __attribute ((packed)) NightModeConfig {
    bool enabled = false;

    uint16_t brightness = 10;

    uint32_t start_time = 0;
    uint32_t end_time = (uint32_t) 10 * 60 * 60;
    uint16_t switch_interval = (uint32_t) 15 * 60;
};

struct __attribute ((packed)) Config {
    bool power = true;
    bool rgb_mode = true;

    uint16_t brightness = 2048;

    uint32_t color = ~0u;
    uint32_t calibration = ~0u;

    NightModeConfig night_mode;
};
