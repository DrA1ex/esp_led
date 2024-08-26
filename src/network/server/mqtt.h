#include <AsyncMqttClient.h>

#include "application.h"

enum class MqttServerState : uint8_t {
    UNINITIALIZED,
    CONNECTING,
    CONNECTED,
    DISCONNECTED
};

class MqttServer {
    AsyncMqttClient _mqttClient;

    Application &_app;

    MqttServerState _state = MqttServerState::UNINITIALIZED;
    unsigned long _state_change_time = 0;
    unsigned long _last_connection_attempt_time = 0;

public:
    explicit MqttServer(Application &app);

    void begin();

    void handle_connection();

    void notify_brightness(uint16_t value);
    void notify_power(bool value);

protected:
    void _on_connect(bool sessionPresent);
    void _on_disconnect(AsyncMqttClientDisconnectReason reason);
    void _on_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

    void _subscribe(const char *topic, uint8_t qos);
    void _publish(const char *topic, uint8_t qos, const char *payload, size_t length);

    void _change_state(MqttServerState state);
    void _connect();

    void _on_property_changed(NotificationParameter param);
};


