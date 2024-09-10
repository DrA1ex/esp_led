import {PacketType} from "./cmd.js";

export const PropertyConfig = [{
    key: "general", section: "General", props: [
        {key: "power", title: "Power", type: "trigger", kind: "Boolean", cmd: PacketType.POWER},
        {key: "brightness", title: "Brightness", type: "wheel", min: 1, limit: 16383, kind: "Uint16", cmd: PacketType.BRIGHTNESS},
        {key: "color", title: "Color", type: "color", kind: "Uint32", cmd: PacketType.COLOR, visibleIf: "rgb_mode"},
        {key: "calibration", title: "Calibration", type: "color", kind: "Uint32", cmd: PacketType.CALIBRATION, visibleIf: "rgb_mode"},
        {key: "rgb_mode", type: "skip"},
    ],
}, {
    key: "night_mode", section: "Night Mode", lock: true, props: [
        {key: "nightMode.enabled", title: "Enabled", type: "trigger", kind: "Boolean", cmd: PacketType.NIGHT_MODE_ENABLED},
        {
            key: "nightMode.brightness",
            title: "Brightness",
            type: "wheel",
            limit: 16383,
            kind: "Uint16",
            cmd: PacketType.NIGHT_MODE_BRIGHTNESS
        },
        {key: "nightMode.startTime", title: "Start Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_START},
        {key: "nightMode.endTime", title: "End Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_END},
        {key: "nightMode.switchInterval", title: "Switch Interval", type: "time", kind: "Uint16", cmd: PacketType.NIGHT_MODE_INTERVAL},
    ]
}];
