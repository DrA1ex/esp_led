#pragma once

#include <functional>

#include "config.h"
#include "night_mode.h"
#include "misc/storage.h"

enum class NotificationParameter : uint8_t {
    POWER,
    BRIGHTNESS
};

typedef std::function<void(NotificationParameter)> ParameterChangedCallback;

class Application {
    ParameterChangedCallback _parameter_changed_cb = nullptr;

public:
    Storage<Config> &config_storage;

    Config &config;
    NightModeManager &night_mode_manager;

    unsigned long state_change_time = 0;
    AppState state = AppState::UNINITIALIZED;

    explicit Application(Storage<Config> &config_storage, NightModeManager &night_mode_manager);

    void change_state(AppState s);

    void load();
    void update();

    uint16_t brightness() const;

    void set_power(bool on);
    void set_brightness(uint16_t value);

    void restart();

    inline void notify_parameter_changed(NotificationParameter param) { if (_parameter_changed_cb) _parameter_changed_cb(param); }

    inline void on_parameter_changed(ParameterChangedCallback fn) { _parameter_changed_cb = fn; }
};