#pragma once

#include <cstdint>

#include "utils/enum.h"

MAKE_ENUM(PacketType, uint8_t,
    POWER, 0x01,
    BRIGHTNESS, 0x02,

    COLOR, 0x10,
    CALIBRATION, 0x11,

    NIGHT_MODE_ENABLED, 0x20,
    NIGHT_MODE_START, 0x21,
    NIGHT_MODE_END, 0x22,
    NIGHT_MODE_INTERVAL, 0x23,
    NIGHT_MODE_BRIGHTNESS, 0x24,

    GET_CONFIG, 0xa0,
    RESTART, 0xb0,
)
