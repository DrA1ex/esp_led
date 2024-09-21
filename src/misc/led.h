#pragma once

#include <cstdint>

#include "constants.h"

class LedController {
    bool _rgb_mode;

    uint8_t _r_pin, _g_pin, _b_pin;

    uint16_t _brightness = PWM_MAX_VALUE;
    uint16_t _color_r{0}, _color_g{0}, _color_b{0};

    uint32_t _color = 0xffffff;
    uint32_t _calibration = 0xffffff;

public:
    LedController(uint8_t pin);
    LedController(uint8_t r_pin, uint8_t g_pin, uint8_t b_pin);

    void begin();

    void set_brightness(uint16_t value);
    void set_color(uint32_t color);
    void set_calibration(uint32_t calibration);

    inline bool rgb_mode() { return _rgb_mode; }
    inline uint16_t brightness() { return _brightness; }

private:
    void _apply_rgb_brightness(uint16_t brightness);
    void _analog_write();

    void _load_calibration(uint32_t color, uint32_t calibration);
    uint16_t _convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit);
    uint8_t _apply_gamma(uint8_t color, float gamma = GAMMA);
};