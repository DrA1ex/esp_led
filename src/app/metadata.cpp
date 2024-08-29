#include "metadata.h"

#include "constants.h"

std::map<PacketType, PropertyMetadata> PacketTypeMetadataMap = {
        {
                PacketType::POWER_ON,
                {
                        NotificationProperty::POWER,                 PacketType::POWER_ON,
                        offsetof(Config, power),                      sizeof(Config::power),
                        MQTT_TOPIC_POWER,      MQTT_OUT_TOPIC_POWER,
                }
        },
        {
                PacketType::POWER_OFF,
                {
                        NotificationProperty::POWER,                 PacketType::POWER_OFF,
                        offsetof(Config, power),                      sizeof(Config::power),
                        MQTT_TOPIC_POWER,      MQTT_OUT_TOPIC_POWER,
                }
        },
        {
                PacketType::BRIGHTNESS,
                {
                        NotificationProperty::BRIGHTNESS,            PacketType::BRIGHTNESS,
                        offsetof(Config, brightness),                 sizeof(Config::brightness),
                        MQTT_TOPIC_BRIGHTNESS, MQTT_OUT_TOPIC_BRIGHTNESS,
                }
        },
        {
                PacketType::COLOR,
                {
                        NotificationProperty::COLOR,                 PacketType::COLOR,
                        offsetof(Config, color),                      sizeof(Config::color),
                        MQTT_TOPIC_COLOR,      MQTT_OUT_TOPIC_COLOR,
                }
        },
        {
                PacketType::CALIBRATION,
                {
                        NotificationProperty::CALIBRATION,           PacketType::CALIBRATION,
                        offsetof(Config, calibration),                sizeof(Config::calibration)
                }
        },
        {
                PacketType::NIGHT_MODE_ENABLED,
                {
                        NotificationProperty::NIGHT_MODE_ENABLED,    PacketType::NIGHT_MODE_ENABLED,
                        offsetof(Config, night_mode.enabled),         sizeof(Config::night_mode.enabled)
                }
        },
        {
                PacketType::NIGHT_MODE_START,
                {
                        NotificationProperty::NIGHT_MODE_START,      PacketType::NIGHT_MODE_START,
                        offsetof(Config, night_mode.start_time),      sizeof(Config::night_mode.start_time)
                }
        },
        {
                PacketType::NIGHT_MODE_END,
                {
                        NotificationProperty::NIGHT_MODE_END,        PacketType::NIGHT_MODE_END,
                        offsetof(Config, night_mode.end_time),        sizeof(Config::night_mode.end_time)
                }
        },
        {
                PacketType::NIGHT_MODE_INTERVAL,
                {
                        NotificationProperty::NIGHT_MODE_INTERVAL,   PacketType::NIGHT_MODE_INTERVAL,
                        offsetof(Config, night_mode.switch_interval), sizeof(Config::night_mode.switch_interval)
                }
        },
        {
                PacketType::NIGHT_MODE_BRIGHTNESS,
                {
                        NotificationProperty::NIGHT_MODE_BRIGHTNESS, PacketType::NIGHT_MODE_BRIGHTNESS,
                        offsetof(Config, night_mode.brightness),      sizeof(Config::night_mode.brightness)
                }
        },
};

std::map<NotificationProperty, std::vector<PropertyMetadata>> PropertyMetadataMap =
        _build_property_metadata_map(PacketTypeMetadataMap);

std::map<String, PropertyMetadata> TopicPropertyMetadata =
        _build_topic_property_metadata_map(PacketTypeMetadataMap);

std::map<NotificationProperty, std::vector<PropertyMetadata>> _build_property_metadata_map(
        std::map<PacketType, PropertyMetadata> &packetMapping) {
    std::map<NotificationProperty, std::vector<PropertyMetadata>> result;

    for (auto &[packetType, metadata]: packetMapping) {
        std::vector<PropertyMetadata> &prop = result[metadata.property];
        prop.push_back(metadata);
    }

    return result;
}

std::map<String, PropertyMetadata> _build_topic_property_metadata_map(
        std::map<PacketType, PropertyMetadata> &packetMapping) {
    std::map<String, PropertyMetadata> result;

    for (auto &[packetType, metadata]: packetMapping) {
        result[metadata.mqtt_in_topic] = metadata;
    }

    return result;
}