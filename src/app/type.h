#pragma once

#include <map>

#include <lib/base/application.h>
#include "lib/utils/enum.h"
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
    NIGHT_MODE_BRIGHTNESS, 8,

    SYS_CONFIG_MDNS_NAME, 9,

    SYS_CONFIG_WIFI_MODE, 10,
    SYS_CONFIG_WIFI_SSID, 11,
    SYS_CONFIG_WIFI_PASSWORD, 12,
    SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL, 13,
    SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL, 14,

    SYS_RGB_MODE, 15,
    SYS_CONFIG_LED_R_PIN, 16,
    SYS_CONFIG_LED_G_PIN, 17,
    SYS_CONFIG_LED_B_PIN, 18,

    SYS_CONFIG_POWER_CHANGE_TIMEOUT, 19,
    SYS_CONFIG_WIFI_CONNECT_FLASH_TIMEOUT, 20,

    SYS_CONFIG_TIME_ZONE, 21,

    SYS_CONFIG_WEB_AUTH_ENABLED, 22,
    SYS_CONFIG_WEB_AUTH_USER, 23,
    SYS_CONFIG_WEB_AUTH_PASSWORD, 24,

    SYS_CONFIG_MQTT_ENABLED, 25,
    SYS_CONFIG_MQTT_HOST, 26,
    SYS_CONFIG_MQTT_PORT, 27,
    SYS_CONFIG_MQTT_USER, 28,
    SYS_CONFIG_MQTT_PASSWORD, 29,
    SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS, 30,
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

    define_meta_entry(NotificationProperty::SYS_CONFIG_MDNS_NAME, PacketType::SYS_CONFIG_MDNS_NAME,
        &Config::sys_config, &SysConfig::mdns_name),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WIFI_MODE, PacketType::SYS_CONFIG_WIFI_MODE,
        &Config::sys_config, &SysConfig::wifi_mode),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WIFI_SSID, PacketType::SYS_CONFIG_WIFI_SSID,
        &Config::sys_config, &SysConfig::wifi_ssid),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WIFI_PASSWORD, PacketType::SYS_CONFIG_WIFI_PASSWORD,
        &Config::sys_config, &SysConfig::wifi_password),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL, PacketType::SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL,
        &Config::sys_config, &SysConfig::wifi_connection_check_interval),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL, PacketType::SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL,
        &Config::sys_config, &SysConfig::wifi_max_connection_attempt_interval),

    define_meta_entry(NotificationProperty::SYS_RGB_MODE, PacketType::SYS_RGB_MODE,
        &Config::sys_config, &SysConfig::rgb_mode),

    define_meta_entry(NotificationProperty::SYS_CONFIG_LED_R_PIN, PacketType::SYS_CONFIG_LED_R_PIN,
        &Config::sys_config, &SysConfig::led_r_pin),

    define_meta_entry(NotificationProperty::SYS_CONFIG_LED_G_PIN, PacketType::SYS_CONFIG_LED_G_PIN,
        &Config::sys_config, &SysConfig::led_g_pin),

    define_meta_entry(NotificationProperty::SYS_CONFIG_LED_B_PIN, PacketType::SYS_CONFIG_LED_B_PIN,
        &Config::sys_config, &SysConfig::led_b_pin),

    define_meta_entry(NotificationProperty::SYS_CONFIG_POWER_CHANGE_TIMEOUT, PacketType::SYS_CONFIG_POWER_CHANGE_TIMEOUT,
        &Config::sys_config, &SysConfig::power_change_timeout),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WIFI_CONNECT_FLASH_TIMEOUT, PacketType::SYS_CONFIG_WIFI_CONNECT_FLASH_TIMEOUT,
        &Config::sys_config, &SysConfig::wifi_connect_flash_timeout),

    define_meta_entry(NotificationProperty::SYS_CONFIG_TIME_ZONE, PacketType::SYS_CONFIG_TIME_ZONE,
        &Config::sys_config, &SysConfig::time_zone),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WEB_AUTH_ENABLED, PacketType::SYS_CONFIG_WEB_AUTH_ENABLED,
        &Config::sys_config, &SysConfig::web_auth),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WEB_AUTH_USER, PacketType::SYS_CONFIG_WEB_AUTH_USER,
        &Config::sys_config, &SysConfig::web_auth_user),

    define_meta_entry(NotificationProperty::SYS_CONFIG_WEB_AUTH_PASSWORD, PacketType::SYS_CONFIG_WEB_AUTH_PASSWORD,
        &Config::sys_config, &SysConfig::web_auth_password),

    define_meta_entry(NotificationProperty::SYS_CONFIG_MQTT_ENABLED, PacketType::SYS_CONFIG_MQTT_ENABLED,
        &Config::sys_config, &SysConfig::mqtt),

    define_meta_entry(NotificationProperty::SYS_CONFIG_MQTT_HOST, PacketType::SYS_CONFIG_MQTT_HOST,
        &Config::sys_config, &SysConfig::mqtt_host),

    define_meta_entry(NotificationProperty::SYS_CONFIG_MQTT_PORT, PacketType::SYS_CONFIG_MQTT_PORT,
        &Config::sys_config, &SysConfig::mqtt_port),

    define_meta_entry(NotificationProperty::SYS_CONFIG_MQTT_USER, PacketType::SYS_CONFIG_MQTT_USER,
        &Config::sys_config, &SysConfig::mqtt_user),

    define_meta_entry(NotificationProperty::SYS_CONFIG_MQTT_PASSWORD, PacketType::SYS_CONFIG_MQTT_PASSWORD,
        &Config::sys_config, &SysConfig::mqtt_password),

    define_meta_entry(NotificationProperty::SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS, PacketType::SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS,
        &Config::sys_config, &SysConfig::mqtt_convert_brightness),
};
