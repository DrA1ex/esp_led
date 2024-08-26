import {PacketType} from "./network/cmd.js";
import {BinaryParser} from "./misc/binary_parser.js";
import {EventEmitter} from "./misc/event_emitter.js";
import {Properties} from "./props.js";


export class Config extends EventEmitter {
    static LOADED = "config_loaded";
    static PROPERTY_CHANGED = "config_prop_changed";

    #ws;

    power;
    brightness;
    nightMode;

    constructor(ws) {
        super();

        this.#ws = ws;
    }

    async load() {
        const {data} = await this.#ws.request(PacketType.GET_CONFIG);
        const parser = new BinaryParser(data.buffer, data.byteOffset);

        this.power = parser.readBoolean();
        this.brightness = parser.readUInt16();

        this.nightMode = {
            enabled: parser.readBoolean(),
            brightness: parser.readUInt16(),

            startTime: parser.readUInt32(),
            endTime: parser.readUInt32(),
            switchInterval: parser.readUInt16(),
        };

        this.emitEvent(Config.LOADED);
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

    setProperty(key, value) {
        if (!Properties[key]) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const oldValue = this.getProperty(key);

        this.#setProperty(key, value);
        this.emitEvent(Config.PROPERTY_CHANGED, {key, value, oldValue});
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