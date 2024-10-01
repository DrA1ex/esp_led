import {PacketType} from "./cmd.js";
import {PWM_MAX_VALUE, TEMPERATURE_MAX_VALUE} from "./sys_constants.js";

/**@type {PropertiesConfig} */
export const PropertyConfig = [{
    key: "general", section: "General", props: [
        {key: "singleLedMode", type: "skip"},
        {key: "rgbMode", type: "skip"},
        {key: "cctMode", type: "skip"},
        {key: "showTemperature", type: "skip"},

        {key: "power", title: "Power", type: "trigger", kind: "Boolean", cmd: PacketType.POWER},
        {key: "brightness", title: "Brightness", type: "wheel", limit: PWM_MAX_VALUE, kind: "Uint16", cmd: PacketType.BRIGHTNESS},

        {key: "color", title: "Color", type: "color", kind: "Uint32", cmd: PacketType.COLOR, visibleIf: "rgbMode"},
        {key: "calibration", title: "Calibration", type: "color", kind: "Uint32", cmd: PacketType.CALIBRATION, visibleIf: "rgbMode"},

        {
            key: "colorTemperature", title: "Color Temperature", type: "wheel", limit: TEMPERATURE_MAX_VALUE, kind: "Uint16", cmd: PacketType.TEMPERATURE,
            anchor: TEMPERATURE_MAX_VALUE / 2, anchorAmount: 0.02, anchored: true, visibleIf: "cctMode",
            displayConverter: function (value) {
                const percent = value / this.limit * 100;
                let converted;
                if (percent < 50) {
                    converted = (percent - 50);
                } else {
                    converted = (percent - 50);
                }


                let [int, fraction] = converted.toFixed(1).split(".");
                if (converted > 0) int = `+${int}`;
                if (int === "-0") int = "0";

                return fraction === "0" ? int : [int, `.${fraction}`];
            }
        },

        {
            key: "colorTemperatureRgb", title: "Color Temperature", type: "wheel", limit: TEMPERATURE_MAX_VALUE, kind: "Uint16", cmd: PacketType.TEMPERATURE,
            visibleIf: "rgbMode",
            displayConverter: function (value) {
                const {sysConfig: {ledMinTemperature, ledMaxTemperature}} = window.__app.app.config

                const factor = value / this.limit;
                const result = ledMinTemperature + (ledMaxTemperature - ledMinTemperature) * factor;
                return result.toFixed(0);
            }
        },
    ],
}, {
    key: "night_mode", section: "Night Mode", collapse: true, props: [
        {key: "nightMode.enabled", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.NIGHT_MODE_ENABLED},
        {key: "nightMode.brightness", title: "Brightness", type: "wheel", limit: PWM_MAX_VALUE, kind: "Uint16", cmd: PacketType.NIGHT_MODE_BRIGHTNESS},
        {key: "nightMode.startTime", title: "Start Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_START},
        {key: "nightMode.endTime", title: "End Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_END},
        {key: "nightMode.switchInterval", title: "Switch Interval", type: "time", kind: "Uint16", cmd: PacketType.NIGHT_MODE_INTERVAL},
    ]
}, {
    key: "led_settings", section: "LED Settings", collapse: true, props: [
        {type: "title", label: "LED"},
        {key: "ledType", title: "LED Type", type: "select", kind: "Uint8", cmd: PacketType.SYS_LED_TYPE, list: "ledType"},

        {key: "sysConfig.ledRPin", title: "Red Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_R_PIN, visibleIf: "rgbMode"},
        {key: "sysConfig.ledGPin", title: "Green Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_G_PIN, visibleIf: "rgbMode"},
        {key: "sysConfig.ledBPin", title: "Blue Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_B_PIN, visibleIf: "rgbMode"},

        {key: "sysConfig.ledWPin", title: "Warm Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_R_PIN, visibleIf: "cctMode"},
        {key: "sysConfig.ledCPin", title: "Cold Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_G_PIN, visibleIf: "cctMode"},

        {key: "sysConfig.ledPin", title: "Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_R_PIN, visibleIf: "singleLedMode"},

        {type: "title", label: "LED EXTRA"},
        {key: "sysConfig.ledMinTemperature", title: "Min Temperature", type: "int", kind: "Uint16", cmd: PacketType.SYS_CONFIG_LED_MIN_TEMPERATURE, visibleIf: "showTemperature"},
        {key: "sysConfig.ledMaxTemperature", title: "Max Temperature", type: "int", kind: "Uint16", cmd: PacketType.SYS_CONFIG_LED_MAX_TEMPERATURE, visibleIf: "showTemperature"},
        {key: "sysConfig.ledMinBrightness", title: "Min Brightness", type: "int", kind: "Uint16", cmd: PacketType.SYS_CONFIG_LED_MIN_BRIGHTNESS},

        {type: "title", label: "Actions", extra: {m_top: true}},
        {key: "apply_led_config", type: "button", label: "Apply Settings"}
    ]
},{
    key: "settings", section: "Settings", collapse: "true", props: [
        {key: "sysConfig.mdnsName", title: "mDNS Name", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MDNS_NAME},

        {type: "title", label: "WiFi"},
        {key: "sysConfig.wifiMode", title: "Mode", type: "select", kind: "Uint8", cmd: PacketType.SYS_CONFIG_WIFI_MODE, list: "wifiMode"},
        {key: "sysConfig.wifiSsid", title: "WiFi SSID", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WIFI_SSID},
        {key: "sysConfig.wifiPassword", title: "WiFi Password", type: "password", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WIFI_PASSWORD},

        {type: "title", label: "WiFi Extra"},
        {key: "sysConfig.wifiConnectionCheckInterval", title: "Connection Check Interval", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL},
        {key: "sysConfig.wifiMaxConnectionAttemptInterval", title: "Max Connection Attempt Interval", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL},

        {type: "title", label: "Button"},
        {key: "sysConfig.button_enabled", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_CONFIG_BUTTON_ENABLED},
        {key: "sysConfig.button_pin", title: "Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_BUTTON_PIN, min: 0, max: 32},
        {key: "sysConfig.button_high_state", title: "High State", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_CONFIG_BUTTON_HIGH_STATE},

        {type: "title", label: "SYSTEM EXTRA"},
        {key: "sysConfig.powerChangeTimeout", title: "Power Change Timeout", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_POWER_CHANGE_TIMEOUT},
        {key: "sysConfig.wifiConnectFlashTimeout", title: "WiFi Connect Flash Timeout", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_CONNECT_FLASH_TIMEOUT},

        {type: "title", label: "Time"},
        {key: "sysConfig.timeZone", title: "Time Zone", type: "float", kind: "Float32", cmd: PacketType.SYS_CONFIG_TIME_ZONE},

        {type: "title", label: "Web Auth"},
        {key: "sysConfig.webAuth", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_CONFIG_WEB_AUTH_ENABLED},
        {key: "sysConfig.webAuthUser", title: "User", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WEB_AUTH_USER},
        {key: "sysConfig.webAuthPassword", title: "Password", type: "password", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WEB_AUTH_PASSWORD},

        {type: "title", label: "MQTT"},
        {key: "sysConfig.mqtt", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_CONFIG_MQTT_ENABLED},
        {key: "sysConfig.mqttHost", title: "Host", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MQTT_HOST},
        {key: "sysConfig.mqttPort", title: "Port", type: "int", kind: "Uint16", cmd: PacketType.SYS_CONFIG_MQTT_PORT},
        {key: "sysConfig.mqttUser", title: "User", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MQTT_USER},
        {key: "sysConfig.mqttPassword", title: "Password", type: "password", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MQTT_PASSWORD},

        {type: "title", label: "MQTT Extra"},
        {key: "sysConfig.mqttConvertBrightness", title: "Convert Brightness", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_CONFIG_MQTT_CONVERT_BRIGHTNESS},

        {type: "title", label: "Actions", extra: {m_top: true}},
        {key: "apply_sys_config", type: "button", label: "Apply Settings"}
    ],
}];
