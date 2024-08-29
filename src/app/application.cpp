#include "application.h"

#include "utils/math.h"

Application::Application(Storage<Config> &config_storage, NightModeManager &night_mode_manager) :
        config_storage(config_storage), config(config_storage.get()), night_mode_manager(night_mode_manager) {}

void Application::load() {
#if RGB_MODE == 1
    _color_r = _convert_color(config.color, config.calibration, 16);
    _color_g = _convert_color(config.color, config.calibration, 8);
    _color_b = _convert_color(config.color, config.calibration, 0);
#endif

    set_brightness(config.power ? brightness() : PIN_DISABLED);
}

void Application::update() {
    config_storage.save();

    load();
}

void Application::change_state(AppState s) {
    state_change_time = millis();
    state = s;

    D_PRINTF("Change app state: %u\n", (uint8_t) s);
}

void Application::set_power(bool on) {
    config.power = on;

    D_PRINTF("Turning Power: %s\n", config.power ? "ON" : "OFF");

    if (state != AppState::INITIALIZATION) {
        change_state(config.power ? AppState::TURNING_ON : AppState::TURNING_OFF);
    }

    config_storage.save();
}

void Application::restart() {
    if (config_storage.is_pending_commit()) config_storage.force_save();

    D_PRINT("Restarting");

    ESP.restart();
}


void Application::set_brightness(uint16_t value) {
    auto brightness = DAC_MAX_VALUE - (uint16_t) floor(
            log10f(10 - (float) value * 9 / DAC_MAX_VALUE) * DAC_MAX_VALUE);

#if RGB_MODE == 1
    analogWrite(LED_R_PIN, (uint32_t) _color_r * brightness / DAC_MAX_VALUE);
    analogWrite(LED_G_PIN, (uint32_t) _color_g * brightness / DAC_MAX_VALUE);
    analogWrite(LED_B_PIN, (uint32_t) _color_b * brightness / DAC_MAX_VALUE);
#else
    analogWrite(LED_PIN, brightness);
#endif
}

uint16_t Application::brightness() const {
    uint16_t result = night_mode_manager.is_night_time()
                      ? night_mode_manager.get_brightness() : config.brightness;

    return std::min(DAC_MAX_VALUE, result);
}

#if RGB_MODE == 1
uint16_t Application::_convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit) {
    uint8_t color = (color_data >> bit) & 0xff;
    uint8_t calibration = (calibration_data >> bit) & 0xff;

    uint8_t calibrated_color = (uint16_t) color * calibration / 255;
    return map16(_apply_gamma(calibrated_color), 255, DAC_MAX_VALUE);
}

uint8_t Application::_apply_gamma(uint8_t color, float gamma) {
    // FastLED: colorutils.cpp@1239
    if (gamma == 1.0f) return color;

    float orig;
    float adj;
    orig = (float) (color) / (255.0);
    adj = pow(orig, gamma) * (255.0);
    uint8_t result = (uint8_t) (adj);
    if ((color > 0) && (result == 0)) {
        result = 1; // never gamma-adjust a positive number down to zero
    }

    return result;
}
#endif
