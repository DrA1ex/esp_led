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

    bool _rgb_mode {false};
    uint16_t _color_r {0}, _color_g {0}, _color_b {0};

public:
    unsigned long state_change_time = 0;
    AppState state = AppState::UNINITIALIZED;

    explicit Application(Storage<Config> &config_storage, NightModeManager &night_mode_manager, Timer &timer)
        : AbstractApplication(PacketTypeMetadataMap),
          _config_storage(config_storage),
          _night_mode_manager(night_mode_manager),
          _timer(timer) {}

    inline ConfigT &config() override { return _config_storage.get(); }
    inline SysConfig &sys_config() { return config().sys_config; }

    void begin();

    void change_state(AppState s);

    uint16_t brightness();
    void set_brightness(uint16_t value);

    void set_power(bool on);
    void restart();

    void load();
    void update();

private:
    void _handle_property_change(NotificationProperty type);
    void _apply_rgb_brightness(uint16_t brightness);
    void _load_calibration();

    uint16_t _convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit);
    uint8_t _apply_gamma(uint8_t color, float gamma = GAMMA);
};
