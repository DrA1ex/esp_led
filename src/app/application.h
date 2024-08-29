#pragma once

#include "config.h"
#include "metadata.h"
#include "night_mode.h"

#include "misc/event_topic.h"
#include "misc/storage.h"

class Application {
public:
    Storage<Config> &config_storage;

    Config &config;
    NightModeManager &night_mode_manager;

    EventTopic<NotificationProperty> e_property_changed{};

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

    inline void notify_parameter_changed(void *sender, NotificationProperty param, void *arg = nullptr) {
        e_property_changed.publish(sender, param, arg);
    }

private:
#if RGB_MODE == 1
    uint16_t _color_r;
    uint16_t _color_g;
    uint16_t _color_b;

    uint16_t _convert_color(uint32_t color_data, uint32_t calibration_data, uint8_t bit);
    uint8_t _apply_gamma(uint8_t color, float gamma = GAMMA);
#endif
};