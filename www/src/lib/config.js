import {EventEmitter} from "./misc/event_emitter.js";
import {BinaryParser} from "./misc/binary_parser.js";


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
     * @abstract
     *
     * @returns {number}
     */
    get cmd() {throw new Error("Not implemented");}

    /**
     * @abstract
     *
     * @param {BinaryParser} parser
     * @returns {void}
     */
    parse(parser) { throw new Error("Not implemented"); }

    /**
     * @param {PropertyConfigMap} propertyMap
     */
    constructor(propertyMap) {
        super();

        this.#propertyMap = propertyMap;
    }

    /**
     * @param {WebSocketInteraction} ws
     * @return {Promise<void>}
     */
    async load(ws) {
        const {data} = await ws.request(this.cmd);

        const parser = new BinaryParser(data.buffer, data.byteOffset);
        this.parse(parser);

        this.emitEvent(AppConfigBase.Event.Loaded);
    }

    getProperty(key) {
        const prop = this.propertyMap[key];
        if (!prop) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const value = prop.key.split(".").reduce((obj, key) => obj[key], this);
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
    }
}
