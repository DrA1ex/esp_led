#pragma once

#include "app/config.h"
#include "lib/debug.h"

class NtpTime;

class NightModeManager {
    static constexpr int32_t FACTOR_FADE_UPDATE_PERIOD_MS = 30;
    static constexpr int32_t FACTOR_UPDATE_PERIOD_MS = 1000;

    const Config &_config;

    bool _is_night = false;

    unsigned long _next_start_fade_time = 0;
    unsigned long _next_start_time = 0;

    unsigned long _next_end_time = 0;
    unsigned long _next_end_fade_time = 0;

    unsigned long _last_fade_factor_update = 0;
    float _fade_factor = 0;

    bool _need_update_parameters = false;

public:
    explicit NightModeManager(const Config &config);

    void handle_night(const NtpTime &ntp_time);

    [[nodiscard]] uint16_t get_brightness() const;
    [[nodiscard]] inline bool is_night_time() const { return _config.night_mode.enabled && _is_night; }

    void reset();

private:
    void _update_next_night_time(const NtpTime &ntp_time);
    void _update_night_flag(unsigned long now);

    void _update_fade_factor(unsigned long now);
};
