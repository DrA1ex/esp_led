#include "application.h"

Application::Application(Storage<Config> &config_storage, NightModeManager &night_mode_manager) :
        config_storage(config_storage), config(config_storage.get()), night_mode_manager(night_mode_manager) {}

void Application::load() {
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
    analogWrite(LED_PIN, brightness);
}

uint16_t Application::brightness() const {
    uint16_t result = night_mode_manager.is_night_time()
                      ? night_mode_manager.get_brightness() : config.brightness;


    return std::min(DAC_MAX_VALUE, result);
}
