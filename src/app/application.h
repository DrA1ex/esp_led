#pragma once

#include "lib/base/application.h"
#include "lib/misc/storage.h"

#include "config.h"
#include "constants.h"
#include "night_mode.h"
#include "type.h"

class Application : public AbstractApplication<Config, AppPropertyMetadata> {
    Storage<Config> &_config_storage;
    NightModeManager &_night_mode_manager;
    Timer &_timer;

    bool _rgb_mode {};

public:
    inline ConfigT &config() override { return _config_storage.get(); }
    inline SysConfig &sys_config() { return config().sys_config; }

    unsigned long state_change_time = 0;
    AppState state = AppState::UNINITIALIZED;

    explicit Application(Storage<Config> &config_storage, NightModeManager &night_mode_manager, Timer &timer);

    void begin();

    void change_state(AppState s);

    void load();
    void update();

    uint16_t brightness();

    void set_power(bool on);
    void set_brightness(uint16_t value);

    void restart();

private:
    uint16_t _color_r = 0;
    uint16_t _color_g = 0;
    uint16_t _color_b = 0;

    uint16_t _convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit);
    uint8_t _apply_gamma(uint8_t color, float gamma = GAMMA);
};
