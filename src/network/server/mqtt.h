#pragma once

#include "lib/network/server/mqtt.h"

#include "app/application.h"

class AppMqttServer : public MqttServer<Application> {
public:
    explicit AppMqttServer(Application &app) : MqttServer(app) {}

protected:
    void _transform_topic_payload(const String &topic, String &payload) override;
    void _transform_topic_notification(const MetaPropT &meta, String &payload) override;

    void _after_message_process(const MetaPropT &meta) override;
};
