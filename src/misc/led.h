#pragma once

#include <cstdint>

#include "constants.h"
#include "app/config.h"

class LedController {
    LedType _led_type;
    uint16_t _brightness = PWM_MAX_VALUE;

    union {
        struct {
            uint8_t _led_pin;
        };

        struct {
            uint8_t _w_pin, _c_pin;
            uint16_t _c_brightness, _w_brightness;
            uint16_t _color_temperature;
        };

        struct {
            uint8_t _r_pin{0}, _g_pin{0}, _b_pin{0};
            uint16_t _color_r{0}, _color_g{0}, _color_b{0};
            uint32_t _color{0};
            uint32_t _calibration{0};
        };
    };

public:
    explicit LedController(uint8_t pin);
    LedController(uint8_t c_pin, uint8_t w_pin);
    LedController(uint8_t r_pin, uint8_t g_pin, uint8_t b_pin);

    void begin();

    void set_brightness(uint16_t value);
    void set_color(uint32_t color);
    void set_calibration(uint32_t calibration);
    void set_temperature(uint16_t temperature);

    [[nodiscard]] inline LedType led_type() const { return _led_type; }
    [[nodiscard]] inline uint16_t brightness() const { return _brightness; }

private:
    void _apply_rgb_brightness(uint16_t brightness);
    void _apply_cct_brightness(uint16_t brightness);
    void _analog_write();

    void _load_color_temperature(uint16_t temperature);
    void _load_calibration(uint32_t color, uint32_t calibration);
    uint16_t _convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit);
    uint8_t _apply_gamma(uint8_t color, float gamma = GAMMA);
};
