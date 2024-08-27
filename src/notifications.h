#pragma once

#include <stddef.h>
#include <cstdint>
#include <vector>
#include <map>

#include "config.h"
#include "network/enum.h"

MAKE_ENUM(NotificationProperty, uint8_t,
          POWER, 0,
          BRIGHTNESS, 1,
          NIGHT_MODE_ENABLED, 2,
          NIGHT_MODE_START, 3,
          NIGHT_MODE_END, 4,
          NIGHT_MODE_INTERVAL, 5,
          NIGHT_MODE_BRIGHTNESS, 6
)

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