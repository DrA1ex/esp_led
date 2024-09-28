#pragma once

#include "lib/bootstrap.h"
#include "lib/misc/button.h"
#include "lib/misc/ntp_time.h"

#include "config.h"
#include "metadata.h"
#include "network/api.h"
#include "misc/night_mode.h"
#include "misc/led.h"

class Application {
    std::unique_ptr<Bootstrap<Config, PacketType>> _bootstrap = nullptr;
    std::unique_ptr<ConfigMetadata> _metadata = nullptr;
    std::unique_ptr<NightModeManager> _night_mode_manager = nullptr;
    std::unique_ptr<NtpTime> _ntp_time = nullptr;
    std::unique_ptr<ApiWebServer> _api = nullptr;
    std::unique_ptr<LedController> _led = nullptr;
    std::unique_ptr<Button> _btn = nullptr;

    bool _initialized = false;

    unsigned long _state_change_time = 0;
    AppState _state = AppState::UNINITIALIZED;

    std::map<const AbstractParameter *, PacketType> _parameter_to_packet{};

public:
    inline Config &config() { return _bootstrap->config(); }
    inline SysConfig &sys_config() { return config().sys_config; }

    void begin();
    void event_loop();

    void change_state(AppState s);
    void set_power(bool on);

    void brightness_increase();
    void brightness_decrease();
    void trigger_temperature();

    void load();
    void update();

    inline void restart() { _bootstrap->restart(); }

private:
    void _setup();

    void _bootstrap_state_changed(void *sender, BootstrapState state, void *arg);

    void _app_loop();
    void _service_loop();

    uint16_t _brightness();

    void _handle_property_change(const AbstractParameter *param);
};
