import {EventEmitter} from "./misc/event_emitter.js";
import {BinaryParser} from "./misc/binary_parser.js";
import {SystemPacketType} from "./network/cmd.js";


/**
 * @abstract
 */
export class AppConfigBase extends EventEmitter {
    static Event = {
        Loaded: "config_base_loaded",
        PropertyChanged: "config_base_prop_changed",
    }

    #propertyMap;

    /**
     * @return {PropertyConfigMap}
     */
    get propertyMap() { return this.#propertyMap;}

    /**
     * @returns {number}
     */
    get cmd() {return SystemPacketType.GET_CONFIG;}

    /**
     * @abstract
     *
     * @param {BinaryParser} parser
     * @returns {void}
     */
    parse(parser) { throw new Error("Not implemented"); }

    /** @type {SelectListConfig} */
    lists = {};

    /**
     * @param {PropertiesConfig} propertyConfig
     */
    constructor(propertyConfig) {
        super();

        this.#propertyMap = propertyConfig.reduce((res, section) => {
            for (const prop of section.props ?? []) {
                if (!prop.key) continue;

                if (res[prop.key]) {
                    console.warn(`Key ${prop.key} already exist`);
                    continue;
                }

                res[prop.key] = prop;
            }

            return res;
        }, {});
    }

    /**
     * @param {WebSocketInteraction} ws
     * @return {Promise<void>}
     */
    async load(ws) {
        const {data} = await ws.request(this.cmd);

        const parser = new BinaryParser(data.buffer, data.byteOffset);
        await this.parse(parser);

        this.emitEvent(AppConfigBase.Event.Loaded);
    }

    getProperty(key) {
        const prop = this.propertyMap[key];
        if (!prop) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const value = this.#property(key, (obj, key) => obj[key]);
        return (prop.transform ? prop.transform(value) : value) ?? prop.default;
    }

    setProperty(key, value, sendNotification = true) {
        if (!this.propertyMap[key]) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const oldValue = this.getProperty(key);
        this.#setProperty(key, value);

        if (sendNotification) this.emitEvent(AppConfigBase.Event.PropertyChanged, {key, value, oldValue});
    }

    #setProperty(key, value) {
        let target = this;
        const parts = key.split(".");
        for (let i = 0; i < parts.length - 1; i++) {
            target = target[parts[i]];
        }

        target[parts.at(-1)] = value;

        this.#property(key, (obj, key) => obj[key] = value);
    }

    #property(path, fn) {
        let target = this;
        const parts = path.split(".");
        for (let i = 0; i < parts.length - 1; i++) {
            if (target instanceof Array) {
                target = target.at(Number.parseInt(parts[i]));
            } else {
                target = target[parts[i]];
            }
        }

        return fn(target, parts.at(-1));
    }
}
