import {PacketType} from "./network/cmd.js";

export const PropertyConfig = [{
    key: "general", section: "General", props: [
        {key: "power", title: "Power", type: "trigger", cmd: [PacketType.POWER_ON, PacketType.POWER_OFF]},
        {key: "brightness", title: "Brightness", type: "wheel", min: 1, limit: 16383, kind: "Uint16", cmd: PacketType.BRIGHTNESS},
    ],
}, {
    key: "night_mode", section: "Night Mode", lock: true, props: [
        {key: "nightMode.enabled", title: "Enabled", type: "trigger", cmd: PacketType.NIGHT_MODE_ENABLED},
        {key: "nightMode.brightness", title: "Brightness", type: "wheel", limit: 16383, kind: "Uint16", cmd: PacketType.NIGHT_MODE_BRIGHTNESS},
        {key: "nightMode.startTime", title: "Start Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_START},
        {key: "nightMode.endTime", title: "End Time", type: "time", kind: "Uint32", cmd: PacketType.NIGHT_MODE_END},
        {key: "nightMode.switchInterval", title: "Switch Interval", type: "time", kind: "Uint16", cmd: PacketType.NIGHT_MODE_INTERVAL},
    ]
}];

export const Properties = PropertyConfig.reduce((res, section) => {
    for (const prop of section.props) {
        if (res[prop.key]) {
            console.warn(`Key ${prop.key} already exist`);
            continue;
        }

        res[prop.key] = prop;
    }

    return res;
}, {});