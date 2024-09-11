#include <Arduino.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <LittleFS.h>

#include "constants.h"

#include "lib/misc/event_topic.h"
#include "lib/misc/ntp_time.h"
#include "lib/misc/storage.h"
#include "lib/misc/timer.h"
#include "lib/network/web.h"
#include "lib/network/wifi.h"
#include "lib/network/server/ws.h"
#include "lib/utils/qr.h"
#include "lib/debug.h"

#include "app/config.h"
#include "app/application.h"
#include "app/night_mode.h"

#include "network/server/api.h"
#include "network/server/handler.h"
#include "network/server/mqtt.h"

#include "utils/math.h"

Timer global_timer;

Storage<Config> config_storage(global_timer, "config", STORAGE_CONFIG_VERSION);

NightModeManager night_mode_manager(config_storage.get());
Application app(config_storage, night_mode_manager, global_timer);

WifiManager *wifi_manager;
WebServer web_server(WEB_PORT);

ApiWebServer api_server(app);
AppMqttServer mqtt_server(app);

AppPacketHandler packet_handler(app);
WebSocketServer ws_server(app, packet_handler);

NtpTime ntp_time;

SysConfig *sys_config;
DNSServer *dns_server;

void animation_loop(void *);
void service_loop(void *);

void setup() {
#if defined(DEBUG)
    Serial.begin(74880);
    D_PRINT();

    delay(2000);
#endif

    if (!LittleFS.begin()) {
        D_PRINT("Unable to initialize FS");
    }

    config_storage.begin(&LittleFS);
    sys_config = &config_storage.get().sys_config;

    wifi_manager = new WifiManager(sys_config->wifi_ssid, sys_config->wifi_password,
        sys_config->wifi_connection_check_interval);

    analogWriteResolution(DAC_RESOLUTION);

#if RGB_MODE_SUPPORT == 1
    pinMode(sys_config->led_r_pin, OUTPUT);
    pinMode(sys_config->led_g_pin, OUTPUT);
    pinMode(sys_config->led_b_pin, OUTPUT);

    digitalWrite(sys_config->led_r_pin, PIN_DISABLED);
    digitalWrite(sys_config->led_g_pin, PIN_DISABLED);
    digitalWrite(sys_config->led_b_pin, PIN_DISABLED);
#else
    pinMode(sys_config->led_pin, OUTPUT);
    digitalWrite(sys_config->led_pin, PIN_DISABLED);
#endif

    global_timer.add_interval(animation_loop, 1000 / 60);
    global_timer.add_interval(service_loop, 20);
}

void loop() {
    global_timer.handle_timers();
}

void animation_loop(void *) {
#if defined(DEBUG) && DEBUG_LEVEL <= __DEBUG_LEVEL_VERBOSE
    static unsigned long t = 0;
    static unsigned long ii = 0;
    if (ii % 10 == 0) D_PRINTF("Animation latency: %lu\n", millis() - t);

    t = millis();
    ++ii;
#endif

    switch (app.state) {
        case AppState::UNINITIALIZED:
            break;

        case AppState::INITIALIZATION: {
            if (app.config().power) {
                const auto factor = map16(
                    (millis() - app.state_change_time) % sys_config->wifi_connect_flash_timeout,
                    sys_config->wifi_connect_flash_timeout,
                    DAC_MAX_VALUE
                );

                uint16_t brightness = app.brightness() * cubic_wave16(factor, DAC_MAX_VALUE) / DAC_MAX_VALUE;
                app.set_brightness(brightness);
            }
        }
        break;

        case AppState::TURNING_ON: {
            uint16_t factor = std::min<unsigned long>(DAC_MAX_VALUE,
                (millis() - app.state_change_time) * DAC_MAX_VALUE / sys_config->power_change_timeout);
            uint16_t brightness = (uint16_t) app.brightness() * ease_cubic16(factor, DAC_MAX_VALUE) / DAC_MAX_VALUE;
            app.set_brightness(brightness);

            if (factor == DAC_MAX_VALUE) app.change_state(AppState::STAND_BY);
            break;
        }

        case AppState::TURNING_OFF: {
            uint16_t factor = DAC_MAX_VALUE - std::min<unsigned long>(DAC_MAX_VALUE,
                (millis() - app.state_change_time) * DAC_MAX_VALUE / sys_config->power_change_timeout);
            uint16_t brightness = (uint16_t) app.brightness() * ease_cubic16(factor, DAC_MAX_VALUE) / DAC_MAX_VALUE;
            app.set_brightness(brightness);

            if (factor == 0) app.change_state(AppState::STAND_BY);
            break;
        }
        case AppState::STAND_BY:
            if (app.config().power && night_mode_manager.is_night_time()) {
                auto brightness = night_mode_manager.get_brightness();
                app.set_brightness(brightness);
            }
            break;
    }
}

void service_loop(void *) {
#if defined(DEBUG) && DEBUG_LEVEL <= __DEBUG_LEVEL_VERBOSE
    static unsigned long t = 0;
    static unsigned long ii = 0;
    if (ii % 10 == 0) D_PRINTF("Service latency: %lu\n", millis() - t);

    t = millis();
    ++ii;
#endif

    static auto state = ServiceState::UNINITIALIZED;

    switch (state) {
        case ServiceState::UNINITIALIZED:
            wifi_manager->connect(sys_config->wifi_mode, sys_config->wifi_max_connection_attempt_interval);
            state = ServiceState::WIFI_CONNECT;

            app.change_state(AppState::INITIALIZATION);
            break;

        case ServiceState::WIFI_CONNECT:
            wifi_manager->handle_connection();

            if (wifi_manager->state() == WifiManagerState::CONNECTED) {
                state = ServiceState::INITIALIZATION;
            }

            break;

        case ServiceState::INITIALIZATION:
            if (sys_config->web_auth) {
                web_server.add_handler(new WebAuthHandler(sys_config->web_auth_user, sys_config->web_auth_password));
            }

            if (sys_config->mqtt) {
                mqtt_server.begin(sys_config->mqtt_host, sys_config->mqtt_port,
                    sys_config->mqtt_user, sys_config->mqtt_password);
            }

            api_server.begin(web_server);
            ws_server.begin(web_server);

            web_server.begin(&LittleFS);
            ntp_time.begin(sys_config->time_zone);

            ArduinoOTA.setHostname(sys_config->mdns_name);
            ArduinoOTA.begin();

            app.begin();

            D_PRINT("ESP Ready");

            if (wifi_manager->mode() == WifiMode::AP) {
                dns_server = new DNSServer();
                dns_server->start(53, "*", WiFi.softAPIP());

                D_PRINT("Connect to WiFi:");
                qr_print_wifi_connection(wifi_manager->ssid(), wifi_manager->password());
            } else {
                String url = "http://";
                url += sys_config->mdns_name;
                url += ".local";

                if (web_server.port() != 80) {
                    url += ":";
                    url += web_server.port();
                }

                D_PRINT("Open WebUI:");
                qr_print_string(url.c_str());
            }

            app.change_state(AppState::STAND_BY);
            state = ServiceState::STAND_BY;
            break;

        case ServiceState::STAND_BY: {
            wifi_manager->handle_connection();
            ArduinoOTA.handle();

            if (dns_server) dns_server->processNextRequest();

            night_mode_manager.handle_night(ntp_time);
            if (wifi_manager->mode() == WifiMode::STA) ntp_time.update();

            ws_server.handle_incoming_data();

            if (sys_config->mqtt) mqtt_server.handle_connection();

            break;
        }

        default:;
    }
}
