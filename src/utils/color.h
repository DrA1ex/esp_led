#pragma once

#include <cstdint>
#include <cmath>

inline float _clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

inline uint32_t temperature_to_rgb(float kelvin) {
    // Normalize the temperature to the range of the formula
    float temp = kelvin / 100.0f;
    float r, g, b;

    if (temp <= 66) {
        r = 255;
    } else {
        r = temp - 60;
        r = 329.698727446f * powf(r, -0.1332047592f);
        r = _clamp(r, 0, 255);
    }

    if (temp <= 66) {
        g = temp;
        g = 99.4708025861f * logf(g) - 161.1195681661f;
        g = _clamp(g, 0, 255);
    } else {
        g = temp - 60;
        g = 288.1221695283f * powf(g, -0.0755148492f);
        g = _clamp(g, 0, 255);
    }

    if (temp >= 66) {
        b = 255;
    } else if (temp <= 19) {
        b = 0;
    } else {
        b = temp - 10;
        b = 138.5177312231f * logf(b) - 305.0447927307f;
        b = _clamp(b, 0, 255);
    }

    return (uint32_t) r << 16
        | (uint32_t) g << 8
        | (uint32_t) b;
}
