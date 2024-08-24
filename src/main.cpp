#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>

#include "application.h"
#include "config.h"
#include "debug.h"
#include "night_mode.h"

#include "misc/storage.h"
#include "misc/timer.h"
#include "misc/ntp_time.h"

#include "network/web.h"
#include "network/wifi.h"
#include "network/server/api.h"
#include "network/server/ws.h"

#include "utils/math.h"

Timer global_timer;

Storage<Config> config_storage(global_timer, "config", STORAGE_CONFIG_VERSION);

NightModeManager night_mode_manager(config_storage.get());
Application app(config_storage, night_mode_manager);

WifiManager wifi_manager;
WebServer web_server(WEB_PORT);

ApiWebServer api_server(app);
WebSocketServer ws_server(app);

NtpTime ntp_time;

void animation_loop(void *);
void service_loop(void *);

void setup() {
#if defined(DEBUG)
    Serial.begin(115200);
    delay(2000);
#endif

    if (!LittleFS.begin()) {
        D_PRINT("Unable to initialize FS");
    }

    config_storage.begin(&LittleFS);

    analogWriteResolution(DAC_RESOLUTION);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, PIN_DISABLED);

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
            if (app.config.power) {
                const auto factor = map16(
                        (millis() - app.state_change_time) % WIFI_CONNECT_FLASH_TIMEOUT,
                        WIFI_CONNECT_FLASH_TIMEOUT,
                        DAC_MAX_VALUE
                );

                uint16_t brightness = app.brightness() * cubic_wave16(factor, DAC_MAX_VALUE) / DAC_MAX_VALUE;
                app.set_brightness(brightness);
            }
        }
            break;

        case AppState::TURNING_ON: {
            uint16_t factor = std::min<unsigned long>(
                    DAC_MAX_VALUE, (millis() - app.state_change_time) * DAC_MAX_VALUE / POWER_CHANGE_TIMEOUT);
            uint16_t brightness = (uint16_t) app.brightness() * ease_cubic16(factor, DAC_MAX_VALUE) / DAC_MAX_VALUE;
            app.set_brightness(brightness);

            if (factor == DAC_MAX_VALUE) app.change_state(AppState::STAND_BY);
            break;
        }

        case AppState::TURNING_OFF: {
            uint16_t factor = DAC_MAX_VALUE - std::min<unsigned long>(
                    DAC_MAX_VALUE, (millis() - app.state_change_time) * DAC_MAX_VALUE / POWER_CHANGE_TIMEOUT);
            uint16_t brightness = (uint16_t) app.brightness() * ease_cubic16(factor, DAC_MAX_VALUE) / DAC_MAX_VALUE;
            app.set_brightness(brightness);

            if (factor == 0) app.change_state(AppState::STAND_BY);
            break;
        }
        case AppState::STAND_BY:
            if (app.config.power && night_mode_manager.is_night_time()) {
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

    static ServiceState state = ServiceState::UNINITIALIZED;

    switch (state) {
        case ServiceState::UNINITIALIZED:
            wifi_manager.connect(WIFI_MODE, WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL);
            state = ServiceState::WIFI_CONNECT;

            app.change_state(AppState::INITIALIZATION);
            break;

        case ServiceState::WIFI_CONNECT:
            wifi_manager.handle_connection();

            if (wifi_manager.state() == WifiManagerState::CONNECTED) {
                state = ServiceState::INITIALIZATION;
            }

            break;

        case ServiceState::INITIALIZATION:
#ifdef WEB_AUTH
            web_server.add_handler(new WebAuthHandler());
#endif

            api_server.begin(web_server);
            ws_server.begin(web_server);

            web_server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request) {
                char result[64] = {};

                snprintf(result, sizeof(result), "General:\nHeap: %u\nNow: %lu\n",
                         ESP.getFreeHeap(), millis());

                request->send_P(200, "text/plain", result);
            });

            web_server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
                request->send_P(200, "text/plain", "OK");

                if (config_storage.is_pending_commit()) config_storage.force_save();
                ESP.restart();
            });


            web_server.begin(&LittleFS);
            ntp_time.begin(TIME_ZONE);

            ArduinoOTA.setHostname(MDNS_NAME);
            ArduinoOTA.begin();

            app.load();

            app.change_state(AppState::STAND_BY);
            state = ServiceState::STAND_BY;
            break;

        case ServiceState::STAND_BY: {
            ntp_time.update();
            night_mode_manager.handle_night(ntp_time);

            ArduinoOTA.handle();
            wifi_manager.handle_connection();

            ws_server.handle_incoming_data();
            break;
        }

        default:;
    }
}