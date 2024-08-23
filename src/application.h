#pragma once

#include "config.h"
#include "night_mode.h"
#include "misc/storage.h"

class Application {
public:
    Storage<Config> &config_storage;
    NightModeManager &night_mode_manager;

    Config &config;

    unsigned long state_change_time = 0;
    AppState state = AppState::INITIALIZATION;


    explicit Application(Storage<Config> &config_storage, NightModeManager &night_mode_manager);

    void change_state(AppState s);

    void load();
    void update();

    uint16_t brightness() const;

    void set_power(bool on);
    void set_brightness(uint16_t value);

    void restart();
};