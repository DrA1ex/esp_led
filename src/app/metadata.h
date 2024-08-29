#pragma once

#include <stddef.h>
#include <cstdint>
#include <vector>
#include <map>

#include <Arduino.h>

#include "config.h"
#include "network/cmd.h"

#include "lib/base/application.h"

MAKE_ENUM(NotificationProperty, uint8_t,
          POWER, 0,
          BRIGHTNESS, 1,
          COLOR, 2,
          CALIBRATION, 3,
          NIGHT_MODE_ENABLED, 4,
          NIGHT_MODE_START, 5,
          NIGHT_MODE_END, 6,
          NIGHT_MODE_INTERVAL, 7,
          NIGHT_MODE_BRIGHTNESS, 8
)

using AppPropertyMetadata = PropertyMetadata<NotificationProperty, PacketType>;

extern std::map<PacketType, AppPropertyMetadata> PacketTypeMetadataMap;
extern std::map<NotificationProperty, std::vector<AppPropertyMetadata>> PropertyMetadataMap;
extern std::map<String, AppPropertyMetadata> TopicPropertyMetadata;

std::map<NotificationProperty, std::vector<AppPropertyMetadata>> _build_property_metadata_map(
        std::map<PacketType, AppPropertyMetadata> &packetMapping);

std::map<String, AppPropertyMetadata> _build_topic_property_metadata_map(
        std::map<PacketType, AppPropertyMetadata> &packetMapping);