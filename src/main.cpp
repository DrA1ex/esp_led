#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>

#include "constants.h"
#include "credentials.h"

#include "lib/misc/event_topic.h"
#include "lib/misc/ntp_time.h"
#include "lib/misc/storage.h"
#include "lib/misc/timer.h"

#include "lib/network/web.h"
#include "lib/network/wifi.h"
#include "lib/network/server/ws.h"

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
Application app(config_storage, night_mode_manager);

WifiManager wifi_manager(WIFI_SSID, WIFI_PASSWORD, WIFI_CONNECTION_CHECK_INTERVAL);
WebServer web_server(WEB_PORT);


ApiWebServer api_server(app);
MqttServer mqtt_server(app);

PacketHandler packet_handler(app);
WebSocketServer ws_server(app, packet_handler);

NtpTime ntp_time;

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

    config_storage.get().rgb_mode = RGB_MODE;

    analogWriteResolution(DAC_RESOLUTION);

#if RGB_MODE == 1
    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);

    digitalWrite(LED_R_PIN, PIN_DISABLED);
    digitalWrite(LED_G_PIN, PIN_DISABLED);
    digitalWrite(LED_B_PIN, PIN_DISABLED);
#else
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, PIN_DISABLED);
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
            if constexpr (WEB_AUTH) {
                web_server.add_handler(new WebAuthHandler(WEBAUTH_USER, WEBAUTH_PASSWORD));
            }

            if constexpr (MQTT) mqtt_server.begin(MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASSWORD);

            api_server.begin(web_server);
            ws_server.begin(web_server);

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

            if constexpr (MQTT) mqtt_server.handle_connection();

            break;
        }

        default:;
    }
}