#include "application.h"
#include "lib/utils/meta.h"
#include "utils/math.h"

void Application::begin() {
    _rgb_mode = sys_config().rgb_mode;
    if (_rgb_mode) {
        pinMode(sys_config().led_r_pin, OUTPUT);
        pinMode(sys_config().led_g_pin, OUTPUT);
        pinMode(sys_config().led_b_pin, OUTPUT);
    } else {
        pinMode(sys_config().led_pin, OUTPUT);
    }

    event_property_changed().subscribe(this,
        [this](auto sender, auto type, auto) {
            if (sender != this) _handle_property_change(type);
        });

    load();
}

void Application::_handle_property_change(NotificationProperty type) {
    if (type == NotificationProperty::POWER) {
        set_power(config().power);
    } else if (type >= NotificationProperty::NIGHT_MODE_ENABLED && type <= NotificationProperty::NIGHT_MODE_BRIGHTNESS) {
        _night_mode_manager.reset();
        update();
    } else {
        update();
    }
}

void Application::load() {
    if (_rgb_mode) _load_calibration();
    set_brightness(config().power ? brightness() : PIN_DISABLED);
}

void Application::_load_calibration() {
    _color_r = _convert_color(config().color, config().calibration, 16);
    _color_g = _convert_color(config().color, config().calibration, 8);
    _color_b = _convert_color(config().color, config().calibration, 0);
}

void Application::update() {
    _config_storage.save();
    load();
}

void Application::change_state(AppState s) {
    state_change_time = millis();
    state = s;
    D_PRINTF("Change app state: %u\n", (uint8_t) s);
}

void Application::set_power(bool on) {
    config().power = on;

    D_PRINTF("Turning Power: %s\n", on ? "ON" : "OFF");
    if (state != AppState::INITIALIZATION) {
        change_state(on ? AppState::TURNING_ON : AppState::TURNING_OFF);
    }

    _config_storage.save();
}

void Application::restart() {
    D_PRINTF("Received restart signal. Restarting after %u ms.\r\n", RESTART_DELAY);

    if (_config_storage.is_pending_commit()) _config_storage.force_save();

    _timer.add_timeout([](auto) { ESP.restart(); }, RESTART_DELAY);
}

void Application::set_brightness(uint16_t value) {
    auto brightness = DAC_MAX_VALUE - (uint16_t) floor(
        log10f(10 - (float) value * 9 / DAC_MAX_VALUE) * DAC_MAX_VALUE);

    if (_rgb_mode) {
        _apply_rgb_brightness(brightness);
    } else {
        analogWrite(sys_config().led_pin, brightness);
    }
}

void Application::_apply_rgb_brightness(uint16_t brightness) {
    analogWrite(sys_config().led_r_pin, static_cast<uint32_t>(_color_r * brightness / DAC_MAX_VALUE));
    analogWrite(sys_config().led_g_pin, static_cast<uint32_t>(_color_g * brightness / DAC_MAX_VALUE));
    analogWrite(sys_config().led_b_pin, static_cast<uint32_t>(_color_b * brightness / DAC_MAX_VALUE));
}

uint16_t Application::brightness() {
    uint16_t result;
    if (_night_mode_manager.is_night_time()) {
        result = _night_mode_manager.get_brightness();
    } else {
        result = std::max(sys_config().led_min_brightness, config().brightness);
    }


    return std::min(DAC_MAX_VALUE, result);
}

uint16_t Application::_convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit) {
    uint8_t color = (color_data >> bit) & 0xff;
    uint8_t calibration = (calibration_data >> bit) & 0xff;

    uint8_t calibrated_color = (uint16_t) color * calibration / 255;
    return map16(_apply_gamma(calibrated_color), 255, DAC_MAX_VALUE);
}

uint8_t Application::_apply_gamma(uint8_t color, float gamma) {
    if (gamma == 1.0f) return color;

    auto orig = (float) color / 255.0f;
    auto adj = pow(orig, gamma) * 255.0f;

    auto result = (uint8_t) adj;

    // Avoid gamma-adjusting a positive number to zero
    if (color > 0 && result == 0) return 1;

    return result;
}
