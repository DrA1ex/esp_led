#pragma once

#include <lib/base/metadata.h>
#include <lib/utils/metadata.h>

#include "app/config.h"
#include "app/parameters.h"
#include "network/cmd.h"


DECLARE_META_TYPE(AppMetaProperty, PacketType)

DECLARE_META(NightModeConfigMeta, AppMetaProperty,
    MEMBER(Parameter<bool>, enabled),
    MEMBER(Parameter<uint16_t>, brightness),
    MEMBER(Parameter<uint32_t>, start_time),
    MEMBER(Parameter<uint32_t>, end_time),
    MEMBER(Parameter<uint16_t>, switch_interval)
)

DECLARE_META(SysConfigMeta, AppMetaProperty,
    MEMBER(FixedString, mdns_name),
    MEMBER(Parameter<uint8_t>, wifi_mode),
    MEMBER(FixedString, wifi_ssid),
    MEMBER(FixedString, wifi_password),
    MEMBER(Parameter<uint32_t>, wifi_connection_check_interval),
    MEMBER(Parameter<uint32_t>, wifi_max_connection_attempt_interval),
    MEMBER(Parameter<uint8_t>, led_type),
    MEMBER(Parameter<uint8_t>, led_r_pin),
    MEMBER(Parameter<uint8_t>, led_g_pin),
    MEMBER(Parameter<uint8_t>, led_b_pin),
    MEMBER(Parameter<uint16_t>, led_min_brightness),
    MEMBER(Parameter<uint16_t>, led_min_temperature),
    MEMBER(Parameter<uint16_t>, led_max_temperature),
    MEMBER(Parameter<bool>, button_enabled),
    MEMBER(Parameter<uint8_t>, button_pin),
    MEMBER(Parameter<bool>, button_high_state),
    MEMBER(Parameter<uint32_t>, power_change_timeout),
    MEMBER(Parameter<uint32_t>, wifi_connect_flash_timeout),
    MEMBER(Parameter<float>, time_zone),
    MEMBER(Parameter<bool>, web_auth),
    MEMBER(FixedString, web_auth_user),
    MEMBER(FixedString, web_auth_password),
    MEMBER(Parameter<bool>, mqtt),
    MEMBER(FixedString, mqtt_host),
    MEMBER(Parameter<uint16_t>, mqtt_port),
    MEMBER(FixedString, mqtt_user),
    MEMBER(FixedString, mqtt_password),
    MEMBER(Parameter<bool>, mqtt_convert_brightness)
)

DECLARE_META(DataConfigMeta, AppMetaProperty,
    MEMBER(ComplexParameter<Config>, config),
)

DECLARE_META(ConfigMetadata, AppMetaProperty,
    MEMBER(Parameter<bool>, power),
    MEMBER(BrightnessParameter, brightness),
    MEMBER(Parameter<uint32_t>, color),
    MEMBER(Parameter<uint32_t>, calibration),
    MEMBER(TemperatureParameter, color_temperature),
    SUB_TYPE(NightModeConfigMeta, night_mode),
    SUB_TYPE(SysConfigMeta, sys_config),

    SUB_TYPE(DataConfigMeta, data),
)

inline ConfigMetadata build_metadata(Config &config) {
    return {
        .power = {
            PacketType::POWER,
            MQTT_TOPIC_POWER, MQTT_OUT_TOPIC_POWER,
            &config.power
        },
        .brightness = {
            PacketType::BRIGHTNESS,
            MQTT_TOPIC_BRIGHTNESS, MQTT_OUT_TOPIC_BRIGHTNESS,
            {&config.brightness, config.sys_config}
        },
        .color = {
            PacketType::COLOR,
            MQTT_TOPIC_COLOR, MQTT_OUT_TOPIC_COLOR,
            &config.color
        },
        .calibration = {
            PacketType::CALIBRATION,
            &config.calibration
        },
        .color_temperature = {
            PacketType::TEMPERATURE,
            MQTT_TOPIC_TEMPERATURE,MQTT_OUT_TOPIC_TEMPERATURE,
            {&config.color_temperature, config.sys_config}
        },
        .night_mode = {
            .enabled = {
                PacketType::NIGHT_MODE_ENABLED,
                MQTT_TOPIC_NIGHT_MODE, MQTT_OUT_TOPIC_NIGHT_MODE,
                &config.night_mode.enabled
            },
            .brightness = {
                PacketType::NIGHT_MODE_BRIGHTNESS,
                &config.night_mode.brightness
            },
            .start_time = {
                PacketType::NIGHT_MODE_START,
                &config.night_mode.start_time
            },
            .end_time = {
                PacketType::NIGHT_MODE_END,
                &config.night_mode.end_time
            },
            .switch_interval = {
                PacketType::NIGHT_MODE_INTERVAL,
                &config.night_mode.switch_interval
            }
        },
        .sys_config = {
            .mdns_name = {
                PacketType::SYS_CONFIG_MDNS_NAME,
                {config.sys_config.mdns_name, CONFIG_STRING_SIZE}
            },
            .wifi_mode = {
                PacketType::SYS_CONFIG_WIFI_MODE,
                (uint8_t *) &config.sys_config.wifi_mode
            },
            .wifi_ssid = {
                PacketType::SYS_CONFIG_WIFI_SSID,
                {config.sys_config.wifi_ssid, CONFIG_STRING_SIZE}
            },
            .wifi_password = {
                PacketType::SYS_CONFIG_WIFI_PASSWORD,
                {config.sys_config.wifi_password, CONFIG_STRING_SIZE}
            },
            .wifi_connection_check_interval = {
                PacketType::SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL,
                &config.sys_config.wifi_connection_check_interval
            },
            .wifi_max_connection_attempt_interval = {
                PacketType::SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL,
                &config.sys_config.wifi_max_connection_attempt_interval
            },
            .led_type = {
                PacketType::SYS_LED_TYPE,
                (uint8_t *) &config.sys_config.led_type
            },
            .led_r_pin = {
                PacketType::SYS_CONFIG_LED_R_PIN,
                &config.sys_config.led_r_pin
            },
            .led_g_pin = {
                PacketType::SYS_CONFIG_LED_G_PIN,
                &config.sys_config.led_g_pin
            },
            .led_b_pin = {
                PacketType::SYS_CONFIG_LED_B_PIN,
                &config.sys_config.led_b_pin
            },
            .led_min_brightness = {
                PacketType::SYS_CONFIG_LED_MIN_BRIGHTNESS,
                &config.sys_config.led_min_brightness
            },
            .led_min_temperature = {
                PacketType::SYS_CONFIG_LED_MIN_TEMPERATURE,
                &config.sys_config.led_min_temperature
            },
            .led_max_temperature = {
                PacketType::SYS_CONFIG_LED_MAX_TEMPERATURE,
                &config.sys_config.led_max_temperature
            },
            .button_enabled = {
                PacketType::SYS_CONFIG_BUTTON_ENABLED,
                &config.sys_config.button_enabled
            },
            .button_pin = {
                PacketType::SYS_CONFIG_BUTTON_PIN,
                &config.sys_config.button_pin
            },
            .button_high_state = {
                PacketType::SYS_CONFIG_BUTTON_HIGH_STATE,
                &config.sys_config.button_high_state
            },
            .power_change_timeout = {
                PacketType::SYS_CONFIG_POWER_CHANGE_TIMEOUT,
                &config.sys_config.power_change_timeout
            },
            .wifi_connect_flash_timeout = {
                PacketType::SYS_CONFIG_WIFI_CONNECT_FLASH_TIMEOUT,
                &config.sys_config.wifi_connect_flash_timeout
            },
            .time_zone = {
                PacketType::SYS_CONFIG_TIME_ZONE,
                &config.sys_config.time_zone
            },
            .web_auth = {
                PacketType::SYS_CONFIG_WEB_AUTH_ENABLED,
                &config.sys_config.web_auth
            },
            .web_auth_user = {
                PacketType::SYS_CONFIG_WEB_AUTH_USER,
                {config.sys_config.web_auth_user, CONFIG_STRING_SIZE}
            },
            .web_auth_password = {
                PacketType::SYS_CONFIG_WEB_AUTH_PASSWORD,
                {config.sys_config.web_auth_password, CONFIG_STRING_SIZE}
            },
            .mqtt = {
                PacketType::SYS_CONFIG_MQTT_ENABLED,
                &config.sys_config.mqtt
            },
            .mqtt_host = {
                PacketType::SYS_CONFIG_MQTT_HOST,
                {config.sys_config.mqtt_host, CONFIG_STRING_SIZE}
            },
            .mqtt_port = {
                PacketType::SYS_CONFIG_MQTT_PORT,
                &config.sys_config.mqtt_port
            },
            .mqtt_user = {
                PacketType::SYS_CONFIG_MQTT_USER,
                {config.sys_config.mqtt_user, CONFIG_STRING_SIZE}
            },
            .mqtt_password = {
                PacketType::SYS_CONFIG_MQTT_PASSWORD,
                {config.sys_config.mqtt_password, CONFIG_STRING_SIZE}
            },
            .mqtt_convert_brightness = {
                PacketType::SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS,
                &config.sys_config.mqtt_convert_brightness
            }
        },

        .data{
            .config = ComplexParameter(&config),
        },
    };
}
