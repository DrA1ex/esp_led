#include "led.h"

#include <Arduino.h>
#include "utils/math.h"

LedController::LedController(uint8_t pin) :
        _rgb_mode(false), _r_pin(pin), _g_pin(0), _b_pin(0) {}

LedController::LedController(uint8_t r_pin, uint8_t g_pin, uint8_t b_pin) :
        _rgb_mode(true), _r_pin(r_pin), _g_pin(g_pin), _b_pin(b_pin) {}

void LedController::begin() {
    pinMode(_r_pin, OUTPUT);

    if (_rgb_mode) {
        pinMode(_g_pin, OUTPUT);
        pinMode(_b_pin, OUTPUT);

        _load_calibration(_color, _calibration);
    }
}

void LedController::set_brightness(uint16_t value) {
    _brightness = PWM_MAX_VALUE - (uint16_t) floor(
            log10f(10 - (float) value * 9 / PWM_MAX_VALUE) * PWM_MAX_VALUE);

    _analog_write();
}

void LedController::set_color(uint32_t color) {
    if (!_rgb_mode) return;

    _color = color;
    _load_calibration(_color, _calibration);

    _analog_write();
}

void LedController::set_calibration(uint32_t calibration) {
    if (!_rgb_mode) return;

    _calibration = calibration;
    _load_calibration(_color, _calibration);

    _analog_write();
}

void LedController::_apply_rgb_brightness(uint16_t brightness) {
    analogWrite(_r_pin, (uint32_t) _color_r * brightness / PWM_MAX_VALUE);
    analogWrite(_g_pin, (uint32_t) _color_g * brightness / PWM_MAX_VALUE);
    analogWrite(_b_pin, (uint32_t) _color_b * brightness / PWM_MAX_VALUE);
}

void LedController::_load_calibration(uint32_t color, uint32_t calibration) {
    _color_r = _convert_color(color, calibration, 16);
    _color_g = _convert_color(color, calibration, 8);
    _color_b = _convert_color(color, calibration, 0);
}

uint16_t LedController::_convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit) {
    uint8_t color = (color_data >> bit) & 0xff;
    uint8_t calibration = (calibration_data >> bit) & 0xff;

    uint8_t calibrated_color = (uint16_t) color * calibration / 255;
    return map16(_apply_gamma(calibrated_color), 255, PWM_MAX_VALUE);
}

uint8_t LedController::_apply_gamma(uint8_t color, float gamma) {
    if (gamma == 1.0f) return color;

    auto orig = (float) color / 255.0f;
    auto adj = pow(orig, gamma) * 255.0f;

    auto result = (uint8_t) adj;

    // Avoid gamma-adjusting a positive number to zero
    if (color > 0 && result == 0) return 1;

    return result;
}

void LedController::_analog_write() {
    if (_rgb_mode) {
        _apply_rgb_brightness(_brightness);
    } else {
        analogWrite(_r_pin, _brightness);
    }
}
