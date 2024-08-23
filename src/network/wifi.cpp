#include "wifi.h"

#include <WiFi.h>

#include "constants.h"
#include "debug.h"

void WifiManager::connect(byte mode, unsigned long connection_interval) {
    if (_state == WifiManagerState::CONNECTING) return;

    WiFi.disconnect();

    _mode = mode;
    _connection_interval = connection_interval;
    _state = WifiManagerState::DISCONNECTED;

    switch (_mode) {
        case 0u:
            _connect_ap();
            break;

        case 1u:
            _connect_sta_step();
            break;

        default:;
    }
}

void WifiManager::handle_connection() {
    if (_state == WifiManagerState::CONNECTING) {
        if (_mode == WIFI_STA) _connect_sta_step();

        return;
    }

    if (millis() - _last_connection_check < WIFI_CONNECTION_CHECK_INTERVAL) return;

    _last_connection_check = millis();
    if (WiFi.getMode() == wifi_mode_t::WIFI_MODE_STA && !WiFi.isConnected()) {
        D_PRINT("Wi-Fi connection lost");

        _state = WifiManagerState::DISCONNECTED;
        connect(WIFI_MODE);
    }
}
void WifiManager::_connect_ap() {
    if (_state != WifiManagerState::DISCONNECTED) return;

    WiFi.mode(wifi_mode_t::WIFI_AP);

    String chip_id = String(ESP.getEfuseMac() & 0xffff, HEX);
    chip_id.toUpperCase();

    String ssid = String(WIFI_SSID) + "_" + chip_id;
    WiFi.softAP(ssid, WIFI_PASSWORD);

    D_WRITE("Access point created: ");
    D_PRINT(ssid);

    D_WRITE("IP: ");
    D_PRINT(WiFi.softAPIP());

    _state = WifiManagerState::CONNECTED;
}

void WifiManager::_connect_sta_step() {
    if (_state == WifiManagerState::CONNECTED) return;

    if (_state == WifiManagerState::DISCONNECTED) {
        D_PRINT("Connecting to Wi-Fi...");

        WiFi.mode(wifi_mode_t::WIFI_MODE_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        _state = WifiManagerState::CONNECTING;
        _connection_begin_time = millis();
        return;
    }

    if (WiFi.isConnected()) {
        D_WRITE("Wi-Fi connected! IP address: ");
        D_PRINT(WiFi.localIP());

        _state = WifiManagerState::CONNECTED;
        return;
    }

    if (_connection_interval > 0 && millis() - _connection_begin_time > _connection_interval) {
        D_PRINT("Wi-Fi connection interval exceeded. Switch to AP mode.");

        _state = WifiManagerState::DISCONNECTED;
        connect(0);
    }
}
