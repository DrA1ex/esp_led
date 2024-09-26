#pragma once

#include <lib/base/metadata.h>

#include "app/config.h"
#include "utils/math.h"

class BrightnessParameter : public Parameter<uint16_t> {
    const SysConfig &_config;

public:
    BrightnessParameter(uint16_t *value, const SysConfig &config) : Parameter(value), _config(config) {}

    bool parse(const String &data) override {
        if (_config.mqtt_convert_brightness) {
            if (data.length() == 0) return false;

            uint16_t value = map16(data.toInt(), 100, PWM_MAX_VALUE);
            Parameter::set_value(&value, sizeof(value));
            return true;
        }

        return Parameter::parse(data);
    }

    [[nodiscard]] String to_string() const override {
        if (_config.mqtt_convert_brightness) {
            uint16_t value;
            memcpy(&value, Parameter::get_value(), sizeof(value));
            auto converted = map16(value, PWM_MAX_VALUE, 100);
            return String(converted);
        }

        return Parameter::to_string();
    }
};

class TemperatureParameter : public Parameter<uint16_t> {
    const SysConfig &_config;

public:
    TemperatureParameter(uint16_t *value, const SysConfig &config) : Parameter(value), _config(config) {}

    bool parse(const String &data) override {
        if (data.length() == 0) return false;

        auto parsed = std::max<uint16_t>(data.toInt() - _config.led_min_temperature, 0);
        uint16_t value = map16(parsed, _config.led_max_temperature - _config.led_min_temperature, LED_TEMPERATURE_MAX_VALUE);
        Parameter::set_value(&value, sizeof(value));
        return true;
    }

    [[nodiscard]] String to_string() const override {
        uint16_t value;
        memcpy(&value, Parameter::get_value(), sizeof(value));
        auto converted = _config.led_min_temperature + map16(value, LED_TEMPERATURE_MAX_VALUE,
            _config.led_max_temperature - _config.led_min_temperature);
        return String(converted);
    }
};
