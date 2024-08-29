#include <AsyncMqttClient.h>

#include "app/application.h"

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

protected:
    void _on_connect(bool sessionPresent);
    void _on_disconnect(AsyncMqttClientDisconnectReason reason);
    void _on_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

    void _subscribe(const char *topic, uint8_t qos);
    void _publish(const char *topic, uint8_t qos, const char *payload, size_t length);

    void _process_message(const String &topic, const String &payload);
    void _process_notification(NotificationProperty prop);

    void _change_state(MqttServerState state);
    void _connect();

    template<typename T>
    void _set_value(PropertyMetadata meta, const T &value);

    template<typename T>
    void _notify_value_changed(PropertyMetadata meta);
};
