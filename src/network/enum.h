#pragma once

#include <cctype>

enum class PacketType : uint8_t {
    POWER_ON = 0x01,
    POWER_OFF = 0x02,

    BRIGHTNESS = 0x03,

    NIGHT_MODE_ENABLED = 0x20,
    NIGHT_MODE_START = 0x21,
    NIGHT_MODE_END = 0x22,
    NIGHT_MODE_INTERVAL = 0x23,
    NIGHT_MODE_BRIGHTNESS = 0x24,

    GET_CONFIG = 0xa0
};