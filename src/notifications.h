#pragma once

#include <stddef.h>
#include <cstdint>
#include <vector>
#include <map>

#include "config.h"
#include "network/enum.h"

enum class NotificationProperty : uint8_t {
    POWER,
    BRIGHTNESS,

    NIGHT_MODE_ENABLED,
    NIGHT_MODE_START,
    NIGHT_MODE_END,
    NIGHT_MODE_INTERVAL,
    NIGHT_MODE_BRIGHTNESS,

};

struct PropertyMetadata {
    NotificationProperty property;
    PacketType packet_type;

    uint8_t value_offset;
    uint8_t value_size;
};

extern std::map<PacketType, PropertyMetadata> PacketTypeMetadataMap;
extern std::map<NotificationProperty, std::vector<PropertyMetadata>> PropertyMetadataMap;

std::map<NotificationProperty, std::vector<PropertyMetadata>> _build_property_metadata_map(
        std::map<PacketType, PropertyMetadata> &packetMapping);