#pragma once

#include <cstdint>
#include <cmath>

inline uint16_t smooth16(uint16_t from, uint16_t to, float factor) {
    return from - ((int32_t) from - to) * factor;
}

inline uint16_t tri_wave16(uint16_t value, uint16_t max_value) {
    if (value > max_value / 2) value = max_value - value;

    return std::min(max_value, (uint16_t) (value * 2));
}

inline uint16_t ease_cubic16(uint16_t value, uint16_t max_value) {
    return (uint64_t) value * value * value / ((uint64_t) max_value * max_value);
}

inline uint16_t cubic_wave16(uint16_t value, uint16_t max_value) {
    auto v = tri_wave16(value, max_value);
    return ease_cubic16(v, max_value);
}