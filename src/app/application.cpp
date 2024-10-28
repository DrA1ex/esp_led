#include "application.h"

#include <utils/color.h>

void Application::begin() {
    D_PRINT("Starting application...");

    if (!LittleFS.begin()) {
        D_PRINT("Unable to initialize FS");
    }

    _bootstrap = std::make_unique<Bootstrap<Config, PacketType>>(&LittleFS);

    auto &sys_config = _bootstrap->config().sys_config;
    _bootstrap->begin({
        .mdns_name = sys_config.mdns_name,
        .wifi_mode = sys_config.wifi_mode,
        .wifi_ssid = sys_config.wifi_ssid,
        .wifi_password = sys_config.wifi_password,
        .wifi_connection_timeout = sys_config.wifi_max_connection_attempt_interval,
        .mqtt_enabled = sys_config.mqtt,
        .mqtt_host = sys_config.mqtt_host,
        .mqtt_port = sys_config.mqtt_port,
        .mqtt_user = sys_config.mqtt_user,
        .mqtt_password = sys_config.mqtt_password,
    });

    if (sys_config.led_type == LedType::RGB) {
        _led = std::make_unique<LedController>(sys_config.led_r_pin, sys_config.led_g_pin, sys_config.led_b_pin);
    } else if (sys_config.led_type == LedType::CCT) {
        _led = std::make_unique<LedController>(sys_config.led_r_pin, sys_config.led_g_pin);
    } else {
        _led = std::make_unique<LedController>(sys_config.led_r_pin);
    }

    _led->begin();
    _night_mode_manager = std::make_unique<NightModeManager>(_bootstrap->config());
    _ntp_time = std::make_unique<NtpTime>();

    _api = std::make_unique<ApiWebServer>(*this);
    _api->begin(_bootstrap->web_server());

    if (sys_config.button_enabled) {
        _btn = std::make_unique<Button>(sys_config.button_pin, sys_config.button_high_state);

        _btn->set_on_click([this](auto cnt) {
            if (cnt == 1) {
                this->set_power(!config().power);
            } else if (cnt == 2) {
                trigger_temperature();
            }
        });

        _btn->set_on_hold([this](auto cnt) {
            if (cnt == 1) {
                this->brightness_increase();
            } else if (cnt == 2) {
                this->brightness_decrease();
            }
        });

        _btn->set_on_hold_release([this](auto cnt) {
            if (cnt <= 2) {
                NotificationBus::get().notify_parameter_changed(this, _metadata->brightness.get_parameter());
            }
        });

        _btn->set_hold_call_interval(BTN_HOLD_CALL_INTERVAL);
        _btn->begin();
    }

    _bootstrap->event_state_changed().subscribe(this, [this](auto sender, auto state, auto arg) {
        _bootstrap_state_changed(sender, state, arg);
    });
    _bootstrap->timer().add_interval([this](auto) { _app_loop(); }, APP_LOOP_INTERVAL);

    _setup();
    change_state(AppState::INITIALIZATION);
}

void Application::_setup() {
    NotificationBus::get().subscribe([this](auto sender, auto param) {
        if (sender != this) _handle_property_change(param);
    });

    auto &ws_server = _bootstrap->ws_server();
    auto &mqtt_server = _bootstrap->mqtt_server();

    _metadata = std::make_unique<ConfigMetadata>(build_metadata(config()));
    _metadata->visit([this, &ws_server, &mqtt_server](AbstractPropertyMeta *meta) {
        auto binary_protocol = (BinaryProtocolMeta<PacketType> *) meta->get_binary_protocol();
        if (binary_protocol->packet_type.has_value()) {
            ws_server->register_parameter(*binary_protocol->packet_type, meta->get_parameter());
            VERBOSE(D_PRINTF("WebSocket: Register property %s\r\n", __debug_enum_str(*binary_protocol->packet_type)));
        }

        auto mqtt_protocol = meta->get_mqtt_protocol();
        if (mqtt_protocol->topic_in && mqtt_protocol->topic_out) {
            mqtt_server->register_parameter(mqtt_protocol->topic_in, mqtt_protocol->topic_out, meta->get_parameter());
            VERBOSE(D_PRINTF("MQTT: Register property %s <-> %s\r\n", mqtt_protocol->topic_in, mqtt_protocol->topic_out));
        } else if (mqtt_protocol->topic_out) {
            mqtt_server->register_notification(mqtt_protocol->topic_out, meta->get_parameter());
            VERBOSE(D_PRINTF("MQTT: Register notification -> %s\r\n", mqtt_protocol->topic_out));
        }

        if (binary_protocol->packet_type.has_value()) {
            _parameter_to_packet[meta->get_parameter()] = binary_protocol->packet_type.value();
        }
    });

    ws_server->register_data_request(PacketType::GET_CONFIG, _metadata->data.config);
    ws_server->register_command(PacketType::RESTART, [this] { _bootstrap->restart(); });
}

void Application::event_loop() {
    _bootstrap->event_loop();
}

void Application::_handle_property_change(const AbstractParameter *parameter) {
    auto it = _parameter_to_packet.find(parameter);
    if (it == _parameter_to_packet.end()) return;

    auto type = it->second;
    if (type == PacketType::POWER) {
        set_power(config().power);
    } else if (type == PacketType::TEMPERATURE) {
        if (_led->led_type() == LedType::RGB) {
            uint32_t kelvin = sys_config().led_min_temperature + config().color_temperature *
                (sys_config().led_max_temperature - sys_config().led_min_temperature) / LED_TEMPERATURE_MAX_VALUE;

            config().color = temperature_to_rgb(kelvin);
            NotificationBus::get().notify_parameter_changed(this, _metadata->color.get_parameter());
        }

        update();
    } else if (type >= PacketType::NIGHT_MODE_ENABLED && type <= PacketType::NIGHT_MODE_BRIGHTNESS) {
        _night_mode_manager->reset();
        update();
    } else {
        update();
    }
}

void Application::load() {
    _led->set_brightness(config().power ? _brightness() : PIN_DISABLED);

    if (_led->led_type() == LedType::RGB) {
        _led->set_calibration(config().calibration);
        _led->set_color(config().color);
    } else if (_led->led_type() == LedType::CCT) {
        _led->set_temperature(config().color_temperature);
    }
}

void Application::update() {
    _bootstrap->save_changes();
    load();
}

void Application::change_state(AppState s) {
    _state_change_time = millis();
    _state = s;
    D_PRINTF("Change app state: %s\r\n", __debug_enum_str(s));
}

void Application::set_power(bool on, bool skip_animation) {
    config().power = on;

    D_PRINTF("Turning Power: %s\r\n", on ? "ON" : "OFF");
    if (!skip_animation && _state != AppState::INITIALIZATION) {
        change_state(on ? AppState::TURNING_ON : AppState::TURNING_OFF);
    } else {
        change_state(AppState::STAND_BY);
        load();
    }

    _bootstrap->save_changes();
    NotificationBus::get().notify_parameter_changed(this, _metadata->power.get_parameter());
}

void Application::brightness_increase() {
    if (!config().power) set_power(true, true);

    if (config().brightness == PWM_MAX_VALUE) return;

    config().brightness = std::min<uint16_t>(PWM_MAX_VALUE, config().brightness + max<uint16_t>(1, PWM_MAX_VALUE / BRIGHTNESS_CHANGE_DIVIDER));

    D_PRINTF("Increase brightness: %u\r\n", config().brightness);
    update();
}

void Application::brightness_decrease() {
    if (config().brightness == 0) return;

    config().brightness = std::max(0, config().brightness - std::max<uint16_t>(1, PWM_MAX_VALUE / BRIGHTNESS_CHANGE_DIVIDER));

    D_PRINTF("Decrease brightness: %u\r\n", config().brightness);
    update();
}

void Application::trigger_temperature() {
    if (sys_config().led_type == LedType::SINGLE) return;

    constexpr float value_step = (float) LED_TEMPERATURE_MAX_VALUE / TEMPERATURE_CHANGE_STEPS;
    uint8_t current_step = ceil(config().color_temperature / value_step);
    if (++current_step > TEMPERATURE_CHANGE_STEPS) current_step = 0;

    config().color_temperature = std::min<uint16_t>(current_step * value_step, LED_TEMPERATURE_MAX_VALUE);

    D_PRINTF("Change temperature: %u\r\n", config().color_temperature);

    update();
    NotificationBus::get().notify_parameter_changed(this, _metadata->color_temperature.get_parameter());
}

uint16_t Application::_brightness() {
    uint16_t result;
    if (_night_mode_manager->is_night_time()) {
        result = _night_mode_manager->get_brightness();
    } else {
        result = std::max(sys_config().led_min_brightness, config().brightness);
    }

    return std::min(PWM_MAX_VALUE, result);
}

void Application::_app_loop() {
#if defined(DEBUG) && DEBUG_LEVEL <= __DEBUG_LEVEL_VERBOSE
    static unsigned long t = 0;
    static unsigned long ii = 0;
    if (ii % 10 == 0) D_PRINTF("Animation latency: %lu\r\n", millis() - t);

    t = millis();
    ++ii;
#endif

    switch (_state) {
        case AppState::UNINITIALIZED:
            break;

        case AppState::INITIALIZATION: {
            if (config().power) {
                const auto factor = map16(
                    (millis() - _state_change_time) % sys_config().wifi_connect_flash_timeout,
                    sys_config().wifi_connect_flash_timeout,
                    PWM_MAX_VALUE
                );

                uint16_t brightness = _brightness() * quad_wave16(factor, PWM_MAX_VALUE) / PWM_MAX_VALUE;
                _led->set_brightness(brightness);
            }
        }
        break;

        case AppState::TURNING_ON: {
            uint16_t factor = std::min<unsigned long>(PWM_MAX_VALUE,
                (millis() - _state_change_time) * PWM_MAX_VALUE / sys_config().power_change_timeout);
            uint16_t brightness = (uint16_t) _brightness() * ease_quad16(factor, PWM_MAX_VALUE) / PWM_MAX_VALUE;
            _led->set_brightness(brightness);

            if (factor == PWM_MAX_VALUE) change_state(AppState::STAND_BY);
            break;
        }

        case AppState::TURNING_OFF: {
            uint16_t factor = PWM_MAX_VALUE - std::min<unsigned long>(PWM_MAX_VALUE,
                (millis() - _state_change_time) * PWM_MAX_VALUE / sys_config().power_change_timeout);
            uint16_t brightness = (uint16_t) _brightness() * ease_quad16(factor, PWM_MAX_VALUE) / PWM_MAX_VALUE;
            _led->set_brightness(brightness);

            if (factor == 0) change_state(AppState::STAND_BY);
            break;
        }

        case AppState::STAND_BY:
            if (config().power && _night_mode_manager->is_night_time()) {
                auto brightness = _night_mode_manager->get_brightness();
                _led->set_brightness(brightness);
            }
            break;
    }

    if (_btn) _btn->handle();
}

void Application::_service_loop() {
    _ntp_time->update();
    _night_mode_manager->handle_night(*_ntp_time);
}

void Application::_bootstrap_state_changed(void *sender, BootstrapState state, void *arg) {
    if (state == BootstrapState::INITIALIZING) {
        _ntp_time->begin(TIME_ZONE);

        change_state(AppState::INITIALIZATION);
        load();
    } else if (state == BootstrapState::READY && !_initialized) {
        _initialized = true;

        change_state(AppState::STAND_BY);
        load();

        _bootstrap->timer().add_interval([this](auto) {
            _ntp_time->update();
            _night_mode_manager->handle_night(*_ntp_time);
        }, BOOTSTRAP_SERVICE_LOOP_INTERVAL);
    }
}
