#include "mqtt.h"

#include "utils/math.h"

void AppMqttServer::_transform_topic_payload(const String &topic, String &payload) {
    if constexpr (MQTT_CONVERT_BRIGHTNESS && topic == MQTT_TOPIC_BRIGHTNESS) {
        payload = map16(payload.toInt(), 100, DAC_MAX_VALUE);
    }

    MqttServer::_transform_topic_payload(topic, payload);
}

void AppMqttServer::_transform_topic_notification(const PropertyMetadata<NotificationProperty, PacketType> &meta, String &payload) {
    if constexpr (MQTT_CONVERT_BRIGHTNESS && meta.property == NotificationProperty::BRIGHTNESS) {
        payload = map16(payload.toInt(), DAC_MAX_VALUE, 100);
    }

    MqttServer::_transform_topic_notification(meta, payload);
}

void AppMqttServer::_after_message_process(const PropertyMetadata<NotificationProperty, PacketType> &meta) {
    if (meta.property == NotificationProperty::POWER) {
        app().set_power(app().config().power);
    } else {
        app().load();
    }

    MqttServer::_after_message_process(meta);
}
