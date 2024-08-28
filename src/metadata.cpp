#include "metadata.h"

std::map<PacketType, PropertyMetadata> PacketTypeMetadataMap = {
        {
                PacketType::POWER_ON,
                {
                        NotificationProperty::POWER, PacketType::POWER_ON,
                        offsetof(Config, power), sizeof(Config::power)
                }
        },
        {
                PacketType::POWER_OFF,
                {
                        NotificationProperty::POWER, PacketType::POWER_OFF,
                        offsetof(Config, power), sizeof(Config::power)
                }
        },
        {
                PacketType::BRIGHTNESS,
                {
                        NotificationProperty::BRIGHTNESS, PacketType::BRIGHTNESS,
                        offsetof(Config, brightness), sizeof(Config::brightness)
                }
        },
        {
                PacketType::COLOR,
                {
                        NotificationProperty::COLOR, PacketType::COLOR,
                        offsetof(Config, color), sizeof(Config::color)
                }
        },
        {
                PacketType::CALIBRATION,
                {
                        NotificationProperty::CALIBRATION, PacketType::CALIBRATION,
                        offsetof(Config, calibration), sizeof(Config::calibration)
                }
        },
        {
                PacketType::NIGHT_MODE_ENABLED,
                {
                        NotificationProperty::NIGHT_MODE_ENABLED, PacketType::NIGHT_MODE_ENABLED,
                        offsetof(Config, night_mode.enabled), sizeof(Config::night_mode.enabled)
                }
        },
        {
                PacketType::NIGHT_MODE_START,
                {
                        NotificationProperty::NIGHT_MODE_START, PacketType::NIGHT_MODE_START,
                        offsetof(Config, night_mode.start_time), sizeof(Config::night_mode.start_time)
                }
        },
        {
                PacketType::NIGHT_MODE_END,
                {
                        NotificationProperty::NIGHT_MODE_END, PacketType::NIGHT_MODE_END,
                        offsetof(Config, night_mode.end_time), sizeof(Config::night_mode.end_time)
                }
        },
        {
                PacketType::NIGHT_MODE_INTERVAL,
                {
                        NotificationProperty::NIGHT_MODE_INTERVAL, PacketType::NIGHT_MODE_INTERVAL,
                        offsetof(Config, night_mode.switch_interval), sizeof(Config::night_mode.switch_interval)
                }
        },
        {
                PacketType::NIGHT_MODE_BRIGHTNESS,
                {
                        NotificationProperty::NIGHT_MODE_BRIGHTNESS, PacketType::NIGHT_MODE_BRIGHTNESS,
                        offsetof(Config, night_mode.brightness), sizeof(Config::night_mode.brightness)
                }
        },
};

std::map<NotificationProperty, std::vector<PropertyMetadata>> PropertyMetadataMap =
        _build_property_metadata_map(PacketTypeMetadataMap);


std::map<NotificationProperty, std::vector<PropertyMetadata>> _build_property_metadata_map(
        std::map<PacketType, PropertyMetadata> &packetMapping) {
    std::map<NotificationProperty, std::vector<PropertyMetadata>> result;

    for (auto &[packetType, metadata]: packetMapping) {
        std::vector<PropertyMetadata> &prop = result[metadata.property];
        prop.push_back(metadata);
    }

    return result;
}