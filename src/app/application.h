#pragma once

#include "constants.h"
#include "config.h"
#include "type.h"
#include "night_mode.h"

#include "lib/base/application.h"
#include "lib/misc/storage.h"

class Application : public AbstractApplication<Config, AppPropertyMetadata> {
    Storage<Config> &_config_storage;
    NightModeManager &_night_mode_manager;
    Timer &_timer;

public:
    inline ConfigT &config() override { return _config_storage.get(); }

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
#if RGB_MODE == 1
    uint16_t _color_r;
    uint16_t _color_g;
    uint16_t _color_b;

    uint16_t _convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit);
    uint8_t _apply_gamma(uint8_t color, float gamma = GAMMA);
#endif
};
