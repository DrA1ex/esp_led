#include "mqtt.h"

#include "credentials.h"
#include "sys_constants.h"

#include "utils/math.h"

MqttServer::MqttServer(Application &app) : _app(app) {}

void MqttServer::begin() {
    if (_state != MqttServerState::UNINITIALIZED) return;

    using namespace std::placeholders;

    _mqttClient.onConnect(std::bind(&MqttServer::_on_connect, this, _1));
    _mqttClient.onDisconnect(std::bind(&MqttServer::_on_disconnect, this, _1));
    _mqttClient.onMessage(std::bind(&MqttServer::_on_message, this, _1, _2, _3, _4, _5, _6));

    _mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    _mqttClient.setCredentials(MQTT_USER, MQTT_PASSWORD);

    _connect();
}

void MqttServer::_connect() {
    _change_state(MqttServerState::CONNECTING);

    _mqttClient.connect();
    _last_connection_attempt_time = millis();
}

void MqttServer::_change_state(MqttServerState state) {
    _state = state;
    _state_change_time = millis();
}

void MqttServer::handle_connection() {
    if (_state == MqttServerState::DISCONNECTED && (millis() - _last_connection_attempt_time) > MQTT_RECONNECT_TIMEOUT) {
        D_PRINT("MQTT Reconnecting...");

        _connect();
    }

    if (_state == MqttServerState::CONNECTING && !_mqttClient.connected() && (millis() - _state_change_time) > MQTT_CONNECTION_TIMEOUT) {
        D_PRINT("MQTT Connection timeout");

        _change_state(MqttServerState::DISCONNECTED);
        _mqttClient.disconnect(true);
    }
}

void MqttServer::_on_connect(bool sessionPresent) {
    D_PRINT("MQTT Connected");

    _subscribe(MQTT_TOPIC_BRIGHTNESS, 1);
    _subscribe(MQTT_TOPIC_POWER, 1);

    _change_state(MqttServerState::CONNECTED);
    _last_connection_attempt_time = 0;
}

void MqttServer::_on_disconnect(AsyncMqttClientDisconnectReason reason) {
    D_PRINTF("MQTT Disconnected. Reason %u\n", (uint8_t) reason);

    _change_state(MqttServerState::DISCONNECTED);
}

void MqttServer::_on_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties,
                             size_t len, size_t index, size_t total) {
    D_PRINTF("MQTT Received: %s: \"%.*s\"\n", topic, len, payload);

    String topic_str(topic);

    String payload_str{};
    payload_str.concat(payload, len);

    if (topic_str == MQTT_TOPIC_BRIGHTNESS) {
        auto value = map16(payload_str.toInt(), 100, DAC_MAX_VALUE);
        D_PRINTF("Set brightness %u\n", value);

        _app.config.brightness = value;
        _app.load();
    } else if (topic_str == MQTT_TOPIC_POWER) {
        bool value = payload_str == "1";
        D_PRINTF("Set power %u\n", value);

        _app.set_power(value);
    }
}

void MqttServer::notify_brightness(uint16_t value) {
    auto converted = map16(value, DAC_MAX_VALUE, 100);
    auto converted_str = String(converted);

    _publish(MQTT_OUT_TOPIC_BRIGHTNESS, 1, converted_str.c_str(), converted_str.length());
}

void MqttServer::notify_power(bool value) {
    _publish(MQTT_OUT_TOPIC_POWER, 1, value ? "1" : "0", 1);
}

void MqttServer::_subscribe(const char *topic, uint8_t qos) {
    _mqttClient.subscribe(topic, qos);
    D_PRINTF("MQTT Subscribe: %s\n", topic);
}

void MqttServer::_publish(const char *topic, uint8_t qos, const char *payload, size_t length) {
    if (_state != MqttServerState::CONNECTED) {
        D_PRINTF("MQTT Not connected. Skip message to %s\n", topic);
        return;
    };

    _mqttClient.publish(topic, qos, true, payload, length);

    D_PRINTF("MQTT Publish: %s: \"%.*s\"\n", topic, length, payload);
}
