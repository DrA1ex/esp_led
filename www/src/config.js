import {PacketType} from "./cmd.js";
import {BinaryParser} from "./lib/misc/binary_parser.js";
import {EventEmitter} from "./lib/misc/event_emitter.js";
import {Properties} from "./props.js";


export class Config extends EventEmitter {
    static Event = {
        Loaded: "config_loaded",
        PropertyChanged: "config_prop_changed",
    }

    #ws;

    power;
    rgb_mode;
    brightness;
    color;
    calibration
    nightMode;

    constructor(ws) {
        super();

        this.#ws = ws;
    }

    async load() {
        const {data} = await this.#ws.request(PacketType.GET_CONFIG);
        const parser = new BinaryParser(data.buffer, data.byteOffset);

        this.power = parser.readBoolean();
        this.rgb_mode = parser.readBoolean();

        this.brightness = parser.readUint16();

        this.color = parser.readUint32();
        this.calibration = parser.readUint32();

        this.nightMode = {
            enabled: parser.readBoolean(),
            brightness: parser.readUint16(),

            startTime: parser.readUint32(),
            endTime: parser.readUint32(),
            switchInterval: parser.readUint16(),
        };

        this.emitEvent(Config.Event.Loaded);
    }

    getProperty(key) {
        const prop = Properties[key];
        if (!prop) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const value = prop.key.split(".").reduce((obj, key) => obj[key], this);
        return (prop.transform ? prop.transform(value) : value) ?? prop.default;
    }

    setProperty(key, value, sendNotification = true) {
        if (!Properties[key]) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const oldValue = this.getProperty(key);
        this.#setProperty(key, value);

        if (sendNotification) this.emitEvent(Config.Event.PropertyChanged, {key, value, oldValue});
    }

    #setProperty(key, value) {
        let target = this;
        const parts = key.split(".");
        for (let i = 0; i < parts.length - 1; i++) {
            target = target[parts[i]];
        }

        target[parts.at(-1)] = value;
    }
}