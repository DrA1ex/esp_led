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
#include "network/server/ws.h"

Timer global_timer;

Storage<Config> config_storage(global_timer, "config", STORAGE_CONFIG_VERSION);

NightModeManager night_mode_manager(config_storage.get());
Application app(config_storage, night_mode_manager);

WifiManager wifi_manager;
WebServer web_server(WEB_PORT);

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

    analogWriteResolution(12);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, PIN_DISABLED);

    app.load();

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
        case AppState::INITIALIZATION:
            break;

        case AppState::TURNING_ON: {
            uint16_t factor = std::min<unsigned long>(
                    DAC_MAX_VALUE, (millis() - app.state_change_time) * DAC_MAX_VALUE / POWER_CHANGE_TIMEOUT);
            uint16_t brightness = (uint16_t) app.brightness() * factor / DAC_MAX_VALUE;
            app.set_brightness(brightness);

            if (factor == DAC_MAX_VALUE) app.change_state(AppState::STAND_BY);
            break;
        }

        case AppState::TURNING_OFF: {
            uint16_t factor = DAC_MAX_VALUE - std::min<unsigned long>(
                    DAC_MAX_VALUE, (millis() - app.state_change_time) * DAC_MAX_VALUE / POWER_CHANGE_TIMEOUT);
            uint16_t brightness = (uint16_t) app.brightness() * factor / DAC_MAX_VALUE;
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


            web_server.on("/api/power/status", HTTP_GET, [](AsyncWebServerRequest *request) {
                auto brightness = app.config.brightness * 100 / DAC_MAX_VALUE;

                String out;

                out += "{";
                out += "\"status\": \"ok\",";
                out += "\"value\": " + String(app.config.power) + ",";
                out += "\"brightness\": " + String(brightness);
                out += "}";

                request->send_P(200, "application/json", out.c_str());
            });

            web_server.on("/api/power", HTTP_GET, [](AsyncWebServerRequest *request) {
                bool enabled = request->arg("value") == "1";
                app.set_power(enabled);

                request->send_P(200, "application/json", "{\"status\": \"ok\"}");
            });

            web_server.on("/api/brightness", HTTP_GET, [](AsyncWebServerRequest *request) {
                if (!request->hasArg("value")) {
                    auto value = app.config.brightness * 100 / DAC_MAX_VALUE;
                    auto response = (String("{\"status\": \"ok\", \"value\": ") + String(value) + "}");

                    return request->send_P(200, "application/json", response.c_str());
                }

                auto value = std::min(100, std::max(0, (int) request->arg("value").toInt()));

                app.config.brightness = DAC_MAX_VALUE * value / 100;
                app.load();

                request->send_P(200, "application/json", "{\"status\": \"ok\"}");
            });

            web_server.begin(&LittleFS);
            ntp_time.begin(TIME_ZONE);

            ArduinoOTA.setHostname(MDNS_NAME);
            ArduinoOTA.begin();

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