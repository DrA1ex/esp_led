import {PacketType} from "./cmd.js";

/**@type {PropertiesConfig} */
export const PropertyConfig = [{
    key: "general", section: "General", props: [
        {key: "power", title: "Power", type: "trigger", kind: "Boolean", cmd: PacketType.POWER},
        {key: "brightness", title: "Brightness", type: "wheel", limit: 16383, kind: "Uint16", cmd: PacketType.BRIGHTNESS},
        {key: "color", title: "Color", type: "color", kind: "Uint32", cmd: PacketType.COLOR, visibleIf: "rgbMode"},
        {key: "calibration", title: "Calibration", type: "color", kind: "Uint32", cmd: PacketType.CALIBRATION, visibleIf: "rgbMode"},
    ],
}, {
    key: "night_mode", section: "Night Mode", collapse: true, props: [
        {key: "nightMode.enabled", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.NIGHT_MODE_ENABLED},
        {key: "nightMode.brightness", title: "Brightness", type: "wheel", limit: 16383, kind: "Uint16", cmd: PacketType.NIGHT_MODE_BRIGHTNESS},
        {key: "nightMode.startTime", title: "Start Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_START},
        {key: "nightMode.endTime", title: "End Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_END},
        {key: "nightMode.switchInterval", title: "Switch Interval", type: "time", kind: "Uint16", cmd: PacketType.NIGHT_MODE_INTERVAL},
    ]
}, {
    key: "settings", section: "Settings", collapse: "true", props: [
        {key: "sysConfig.mdnsName", title: "mDNS Name", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_MDNS_NAME},

        {type: "title", label: "WiFi"},
        {key: "sysConfig.wifiMode", title: "Mode", type: "select", kind: "Uint8", cmd: PacketType.SYS_CONFIG_WIFI_MODE, list: "wifiMode"},
        {key: "sysConfig.wifiSsid", title: "WiFi SSID", type: "text", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WIFI_SSID},
        {key: "sysConfig.wifiPassword", title: "WiFi Password", type: "password", kind: "FixedString", maxLength: 32, cmd: PacketType.SYS_CONFIG_WIFI_PASSWORD},

        {type: "title", label: "WiFi Extra"},
        {key: "sysConfig.wifiConnectionCheckInterval", title: "Connection Check Interval", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL},
        {key: "sysConfig.wifiMaxConnectionAttemptInterval", title: "Max Connection Attempt Interval", type: "int", kind: "Uint32", cmd: PacketType.SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL},

        {type: "title", label: "LED"},
        {key: "rgbMode", title: "RGB Mode", type: "trigger", kind: "Boolean", cmd: PacketType.SYS_RGB_MODE},

        {key: "sysConfig.ledRPin", title: "Red Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_R_PIN, visibleIf: "rgbMode"},
        {key: "sysConfig.ledGPin", title: "Green Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_G_PIN, visibleIf: "rgbMode"},
        {key: "sysConfig.ledBPin", title: "Blue Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_B_PIN, visibleIf: "rgbMode"},

        {key: "singleLedMode", type: "skip"},
        {key: "sysConfig.ledPin", title: "Pin", type: "int", kind: "Uint8", cmd: PacketType.SYS_CONFIG_LED_R_PIN, visibleIf: "singleLedMode"},

        {type: "title", label: "LED EXTRA"},
        {key: "sysConfig.ledMinBrightness", title: "Min Brightness", type: "int", kind: "Uint16", cmd: PacketType.SYS_CONFIG_LED_MIN_BRIGHTNESS},
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
},];
