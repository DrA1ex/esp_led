#include <AsyncMqttClient.h>

#include "lib/network/server/mqtt.h"

#include "app/application.h"
#include "utils/math.h"


class MqttServer : public MqttServerBase<Application> {
public:
    explicit MqttServer(Application &app) : MqttServerBase<Application>(app) {}

protected:
    void _transform_topic_payload(const String &topic, String &payload) override;
    void _transform_topic_notification(const MetaPropT &meta, String &payload) override;

    void _after_message_process(const MetaPropT &meta) override;
};
