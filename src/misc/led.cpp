#include "led.h"

#include <Arduino.h>
#include "utils/math.h"

LedController::LedController(uint8_t pin)
    : _led_type(LedType::SINGLE), _led_pin(pin) {}

LedController::LedController(uint8_t r_pin, uint8_t g_pin, uint8_t b_pin)
    : _led_type(LedType::RGB), _r_pin(r_pin), _g_pin(g_pin), _b_pin(b_pin) {

    _color = 0xffffff;
    _calibration = 0xffffff;
    _load_calibration(_color, _calibration);
}

LedController::LedController(uint8_t w_pin, uint8_t c_pin)
    : _led_type(LedType::CCT), _w_pin(w_pin), _c_pin(c_pin) {

    _color_temperature = PWM_MAX_VALUE;
    _load_color_temperature(_color_temperature);
}

void LedController::begin() {
    if (_led_type == LedType::SINGLE) {
        pinMode(_led_pin, OUTPUT);
    } else if (_led_type == LedType::CCT) {
        pinMode(_w_pin, OUTPUT);
        pinMode(_c_pin, OUTPUT);
    } else if (_led_type == LedType::RGB) {
        pinMode(_r_pin, OUTPUT);
        pinMode(_g_pin, OUTPUT);
        pinMode(_b_pin, OUTPUT);
    }
}

void LedController::set_brightness(uint16_t value) {
    uint16_t new_brightness = PWM_MAX_VALUE - (uint16_t) floor(
        log10f(10 - (float) value * 9 / PWM_MAX_VALUE) * PWM_MAX_VALUE);

    if (_brightness == new_brightness) return;

    _brightness = new_brightness;
    _analog_write();
}

void LedController::set_color(uint32_t color) {
    if (_led_type != LedType::RGB || _color == color) return;

    _color = color;
    _load_calibration(_color, _calibration);

    _analog_write();
}

void LedController::set_calibration(uint32_t calibration) {
    if (_led_type != LedType::RGB || _calibration == calibration) return;

    _calibration = calibration;
    _load_calibration(_color, _calibration);

    _analog_write();
}

void LedController::set_temperature(uint16_t temperature) {
    if (_led_type != LedType::CCT || _color_temperature == temperature) return;

    _color_temperature = temperature;
    _load_color_temperature(_color_temperature);
    _analog_write();
}

void LedController::_apply_rgb_brightness(uint16_t brightness) {
    analogWrite(_r_pin, (int32_t) _color_r * brightness / PWM_MAX_VALUE);
    analogWrite(_g_pin, (int32_t) _color_g * brightness / PWM_MAX_VALUE);
    analogWrite(_b_pin, (int32_t) _color_b * brightness / PWM_MAX_VALUE);
}

void LedController::_apply_cct_brightness(uint16_t brightness) {
    analogWrite(_w_pin, (int32_t) _w_brightness * brightness / PWM_MAX_VALUE);
    analogWrite(_c_pin, (int32_t) _c_brightness * brightness / PWM_MAX_VALUE);
}

void LedController::_analog_write() {
    switch (_led_type) {
        case LedType::SINGLE:
            analogWrite(_led_pin, _brightness);
            break;

        case LedType::RGB:
            _apply_rgb_brightness(_brightness);
            break;

        case LedType::CCT:
            _apply_cct_brightness(_brightness);
            break;
    }
}

uint16_t LedController::_convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit) {
    uint8_t color = (color_data >> bit) & 0xff;
    uint8_t calibration = (calibration_data >> bit) & 0xff;

    uint8_t calibrated_color = (uint16_t) color * calibration / 255;
    return map16(_apply_gamma(calibrated_color), 255, PWM_MAX_VALUE);
}

void LedController::_load_calibration(uint32_t color, uint32_t calibration) {
    _color_r = _convert_color(color, calibration, 16);
    _color_g = _convert_color(color, calibration, 8);
    _color_b = _convert_color(color, calibration, 0);
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

void LedController::_load_color_temperature(uint16_t temperature) {
    temperature = std::min<uint16_t>(LED_TEMPERATURE_MAX_VALUE, temperature);

    // Calculate the brightness for the warm white LEDs
    // The brightness decreases as the temperature goes above neutral white
    _w_brightness = (uint16_t) std::min<int32_t>(PWM_MAX_VALUE, std::max<int32_t>(LED_TEMPERATURE_MAX_VALUE - temperature, 0));

    // Calculate the brightness for the cool white LEDs based on the clamped temperature
    // The brightness increases as the temperature approaches neutral white
    _c_brightness = (uint16_t) std::min<int32_t>(PWM_MAX_VALUE, temperature);
}
