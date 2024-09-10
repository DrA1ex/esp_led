#pragma once

#include <map>

#include <lib/base/application.h>
#include <lib/utils/enum.h>
#include <lib/utils/meta.h>

#include "constants.h"
#include "config.h"
#include "network/cmd.h"

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

inline std::map<PacketType, AppPropertyMetadata> PacketTypeMetadataMap = {
    define_meta_entry(NotificationProperty::POWER, PacketType::POWER,
        MQTT_TOPIC_POWER, MQTT_OUT_TOPIC_POWER,
        &Config::power),

    define_meta_entry(NotificationProperty::BRIGHTNESS, PacketType::BRIGHTNESS,
        MQTT_TOPIC_BRIGHTNESS, MQTT_OUT_TOPIC_BRIGHTNESS,
        &Config::brightness),

    define_meta_entry(NotificationProperty::COLOR, PacketType::COLOR,
        MQTT_TOPIC_COLOR, MQTT_OUT_TOPIC_COLOR,
        &Config::color),

    define_meta_entry(NotificationProperty::CALIBRATION, PacketType::CALIBRATION,
        &Config::calibration),

    define_meta_entry(NotificationProperty::NIGHT_MODE_ENABLED, PacketType::NIGHT_MODE_ENABLED,
        MQTT_TOPIC_NIGHT_MODE, MQTT_OUT_TOPIC_NIGHT_MODE,
        &Config::night_mode, &NightModeConfig::enabled),

    define_meta_entry(NotificationProperty::NIGHT_MODE_START, PacketType::NIGHT_MODE_START,
        &Config::night_mode, &NightModeConfig::start_time),

    define_meta_entry(NotificationProperty::NIGHT_MODE_END, PacketType::NIGHT_MODE_END,
        &Config::night_mode, &NightModeConfig::end_time),

    define_meta_entry(NotificationProperty::NIGHT_MODE_INTERVAL, PacketType::NIGHT_MODE_INTERVAL,
        &Config::night_mode, &NightModeConfig::switch_interval),

    define_meta_entry(NotificationProperty::NIGHT_MODE_BRIGHTNESS, PacketType::NIGHT_MODE_BRIGHTNESS,
        &Config::night_mode, &NightModeConfig::brightness),
};
