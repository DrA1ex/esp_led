#pragma once

#include "config.h"
#include "night_mode.h"

#include "misc/event_topic.h"
#include "misc/storage.h"

enum class PropertyChangedKind : uint8_t {
    POWER,
    BRIGHTNESS
};

class Application {
public:
    Storage<Config> &config_storage;

    Config &config;
    NightModeManager &night_mode_manager;

    EventTopic<PropertyChangedKind> e_property_changed{};

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

    inline void notify_parameter_changed(PropertyChangedKind param) {
        e_property_changed.publish(this, param);
    }
};