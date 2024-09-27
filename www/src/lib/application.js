import {AppConfigBase} from "./config.js"
import {
    ButtonControl,
    FrameControl,
    InputControl,
    InputType,
    SelectControl,
    TextControl,
    TriggerControl,
    WheelControl
} from "./control"
import {EventEmitter} from "./misc/event_emitter.js";
import {BinaryParser} from "./misc/binary_parser.js";
import {WebSocketInteraction} from "./network/ws.js";
import * as FunctionUtils from "./utils/function.js";

export class ApplicationCfg {
    throttleInterval = 1000 / 60;

    /**
     * @type {WebSocketConfig}
     */
    webSocketConfig;
}

/**
 * @abstract
 */
export class ApplicationBase extends EventEmitter {
    Event = {
        Connected: "app_base_connected",
        Disconnected: "app_base_disconnected",
        Notification: "app_base_notification",
    }

    #initialized = false;

    /** @type {WebSocketInteraction} */
    #ws;
    /** @type {HTMLElement} */
    #root;

    /** @type {ApplicationCfg} */
    #cfg;

    /** @type {Object<string, PropertyMeta>} */
    #propertyMeta;

    /** @type {Object<string, PropertySectionMeta>} */
    #propertySections;


    /**
     * Property to PropertyMeta map
     * @returns {Object<string, PropertyMeta>}
     */
    get propertyMeta() { return this.#propertyMeta; }

    /**
     * Property to SectionConfig map
     * @returns {Object<string, PropertySectionMeta>}
     */
    get propertySections() { return this.#propertySections; }

    /**
     * Property config
     *
     * @abstract
     * @returns {PropertiesConfig}
     */
    get propertyConfig() {throw new Error("Not implemented");}

    /**
     * Config
     *
     * @abstract
     * @returns {AppConfigBase}
     */
    get config() {throw new Error("Not implemented");}


    /**
     * @type {function (config: any, prop: PropertyConfig, value: 8, oldValue: *): Promise<void>}
     */
    sendChanges;

    /** @type{WebSocketInteraction}*/
    get ws() {return this.#ws;}

    /**
     * @param {string} wsUrl
     * @param {ApplicationCfg?} config
     */
    constructor(wsUrl, config = null) {
        super();

        this.#cfg = Object.assign(new ApplicationCfg(), config || {});

        this.#ws = new WebSocketInteraction(wsUrl, this.#cfg.webSocketConfig);

        this.#ws.subscribe(this, WebSocketInteraction.Event.Connected, this.#handleConnection.bind(this));
        this.#ws.subscribe(this, WebSocketInteraction.Event.Disconnected, () => this.emitEvent(this.Event.Disconnected));
        this.#ws.subscribe(this, WebSocketInteraction.Event.Notification, this.#handleNotification.bind(this));

        this.sendChanges = FunctionUtils.throttle(this.#sendChangesImpl.bind(this), this.#cfg.throttleInterval);

        this.subscribe(this, this.Event.Notification, (_, {key, value}) => {
            this.config.setProperty(key, value, false);

            const {control, title, prop} = this.propertyMeta[key];
            if (prop.type === "skip") return;

            const propValue = this.config.getProperty(key);
            if ("setValue" in control) {
                control.setValue(propValue);
            } else if ("setText" in control) {
                control.setText(propValue);
            }

            if (prop.visibleIf) {
                const visible = !!this.config.getProperty(prop.visibleIf);
                control.setVisibility(visible);
                title?.setVisibility(true);
            }
        });
    }

    async begin(root) {
        this.#root = root;
        await this.#initUi();

        this.#ws.begin();

        window.__app = {
            app: this,
            ws: this.#ws
        }

        document.addEventListener("visibilitychange", () => {
            console.log("Hidden", document.hidden);

            if (document.hidden) {
                this.ws.close();
            } else {
                this.ws.connect();
            }
        });
    }

    /**
     * Initialization code after first WS connection
     *
     * @returns {Promise<void>}
     */
    async initialize() {
        await this.config.load(this.ws);

        console.log("Config", this.config);

        this.config.subscribe(this, AppConfigBase.Event.Loaded, this.#refreshConfig.bind(this));
        this.config.subscribe(this, AppConfigBase.Event.PropertyChanged, this.#onConfigPropChanged.bind(this));

        this.#refreshConfig();
    }

    /**
     * Refresh code after WS reconnection
     *
     * @returns {Promise<void>}
     */
    async refresh() {
        await this.config.load(this.ws);

        console.log("New config", this.config);
    }

    #refreshConfig() {
        const config = this.config

        for (const cfg of this.propertyConfig) {
            if (!cfg.key || !cfg.props?.length) continue;

            const section = this.propertySections[cfg.key];
            if (section.config.visibleIf) {
                if (config.getProperty(section.config.visibleIf)) {
                    section.section.setVisibility(true);
                } else {
                    section.section.setVisibility(false);
                    continue;
                }
            }

            for (const prop of cfg.props) {
                if (!prop.key || prop.type === "skip") continue;

                const {control, title} = section.props[prop.key];

                if (prop.visibleIf) {
                    if (config.getProperty(prop.visibleIf)) {
                        control.setVisibility(true);
                        title?.setVisibility(true);
                    } else {
                        control.setVisibility(false);
                        title?.setVisibility(false);

                        control.setAttribute("data-loading", false);
                        continue;
                    }
                }

                if (prop.type === "select") {
                    control.setOptions(this.config.lists[prop.list].map(v => ({key: v.code, label: v.name})));
                }

                if (control instanceof ButtonControl && prop.cmd) {
                    control.setOnClick(() => this.#sendCommand(prop));
                } else if ("setValue" in control) {
                    const value = config.getProperty(prop.key);
                    control.setValue(value);
                } else if (prop.type === "label") {
                    const value = config.getProperty(prop.key);
                    control.setText(value);
                }

                control.setAttribute("data-loading", false);
                if ("setOnChange" in control) control.setOnChange((value) => config.setProperty(prop.key, value));
            }
        }
    }

    async #onConfigPropChanged(config, {key, value, oldValue}) {
        const propMeta = this.propertyMeta[key];
        if (!propMeta) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const {prop} = propMeta;

        if (typeof prop.min !== "undefined" && value < prop.min) value = prop.min;
        if (typeof prop.limit !== "undefined" && value > prop.limit) value = prop.limit;

        if (value !== oldValue) {
            await this.sendChanges(config, prop, value, oldValue);
        }
    }

    async #handleConnection() {
        try {
            if (!this.#initialized) {
                await this.initialize();
                this.#initialized = true;
            } else {
                await this.refresh();
            }

            this.emitEvent(this.Event.Connected);
        } catch (e) {
            console.error("Unable to finish initialization after connection established.", e);

            this.#ws.close();
            setTimeout(() => this.#ws.connect(), this.#ws.config.connectionTimeoutMaxDelay);
        }
    }

    #handleNotification(sender, packet) {
        return this.#refreshProperty(packet.type, packet.parser());
    }

    #refreshProperty(type, parser) {
        const property = Object.values(this.propertyMeta)
            .find(p =>
                p.prop.cmd instanceof Array ? p.prop.cmd.includes(type) : p.prop.cmd === type
                    && (p.prop.visibleIf ? this.config.getProperty(p.prop.visibleIf) : true));

        if (!property) return console.error("Trying to refresh unknown property", type);

        let value;
        if (property.prop.cmd instanceof Array) {
            value = property.prop.cmd[0] === type;
        } else if (property.prop.kind === "FixedString" && property.prop.maxLength) {
            value = parser.readFixedString(property.prop.maxLength);
        } else {
            value = parser[`read${property.prop.kind}`]();
        }


        this.emitEvent(this.Event.Notification, {key: property.prop.key, value});
    }

    #initUi() {
        const sectionMeta = {};
        const propertyMeta = {};

        for (const cfg of this.propertyConfig) {
            if (!cfg.key || !cfg.props?.length) continue;

            const section = this.#startSection(cfg.section, cfg);

            sectionMeta[cfg.key] = {section, config: cfg, props: {}};

            for (const prop of cfg.props) {
                let title = null
                if (prop.title) {
                    title = this.#createTitle(prop.title);
                    section.appendChild(title);
                }

                let control = null;
                switch (prop.type) {
                    case "trigger":
                        control = new TriggerControl(document.createElement("a"));
                        break;

                    case "wheel":
                        control = new WheelControl(document.createElement("div"), prop.limit);
                        if (prop.displayConverter) control.setDisplayConverter(prop.displayConverter);
                        if (prop.anchor) control.setAnchor(prop.anchor);
                        if (prop.anchorAmount) control.setAnchorAmount(prop.anchorAmount);
                        if (prop.anchored !== undefined) control.setAnchored(prop.anchored);
                        break;

                    case "time":
                        control = new InputControl(document.createElement("input"), InputType.time);
                        break;

                    case "select":
                        control = new SelectControl(document.createElement("div"));
                        break;

                    case "int":
                        control = control = new InputControl(document.createElement("input"), InputType.int);
                        break;

                    case "float":
                        control = control = new InputControl(document.createElement("input"), InputType.float);
                        break;

                    case "text":
                        control = new InputControl(document.createElement("input"), InputType.text);
                        control.setMaxLength(prop.maxLength ?? 255);
                        break;

                    case "password":
                        control = new InputControl(document.createElement("input"), InputType.password);
                        control.setMaxLength(prop.maxLength ?? 255);
                        break;

                    case "color":
                        control = new InputControl(document.createElement("input"), InputType.color);
                        break;

                    case "button":
                        control = new ButtonControl(document.createElement("a"));
                        control.setLabel(prop.label);
                        break;

                    case "title":
                        control = new TextControl(document.createElement("h4"));
                        control.setText(prop.label);
                        break;

                    case "label":
                        control = new TextControl(document.createElement("h4"));
                        control.addClass("label");
                        break;

                    case "separator":
                        control = new FrameControl(document.createElement("hr"));
                        break;

                    case "skip":
                        break;

                    default:
                        console.error("Invalid prop type.", prop)
                }

                if (control) {
                    if (prop.key) control.setAttribute("data-loading", true);

                    if (prop.extra?.m_top) {
                        if (title) {
                            title.addClass("m-top");
                        } else {
                            control.addClass("m-top");
                        }
                    }

                    section.appendChild(control);
                }

                if (prop.key) {
                    const entry = {prop, title, control};
                    sectionMeta[cfg.key].props[prop.key] = entry;
                    propertyMeta[prop.key] = entry;
                }
            }
        }

        this.#propertyMeta = propertyMeta;
        this.#propertySections = sectionMeta;
    }

    #startSection(title, {lock, collapse}) {
        const frame = new FrameControl(document.createElement("div"));
        frame.addClass("section");

        if (lock) {
            const lockBtn = new ButtonControl(document.createElement("a"));
            lockBtn.addClass("lock");
            lockBtn.setOnClick(() => {
                const value = frame.getAttribute("data-locked") === "true";
                frame.setAttribute("data-locked", !value);
            });

            frame.setAttribute("data-locked", true);
            frame.appendChild(lockBtn);
        } else if (collapse) {
            const collapseBtn = new ButtonControl(document.createElement("a"));
            collapseBtn.addClass("collapse");
            collapseBtn.setOnClick(() => {
                const value = frame.getAttribute("data-collapsed") === "true";
                frame.setAttribute("data-collapsed", !value);
            });

            frame.setAttribute("data-collapsed", true);
            frame.appendChild(collapseBtn);
        }

        const sectionTitle = new TextControl(document.createElement("h3"));
        sectionTitle.addClass("section-title");
        sectionTitle.setText(title);

        frame.appendChild(sectionTitle);
        this.#root.appendChild(frame.element);

        return frame;
    }

    #createTitle(title) {
        const titleElement = new TextControl(document.createElement("p"));
        titleElement.setText(title);

        return titleElement;
    }

    async #sendCommand(prop) {
        if (prop.__busy) return;

        const {control} = this.propertyMeta[prop.key];

        prop.__busy = true;
        control.setAttribute("data-saving", true);

        try {
            await this.#ws.request(prop.cmd);
        } catch (e) {
            console.error("Unable to send command", e);
        } finally {
            prop.__busy = false;
            control.setAttribute("data-saving", false);
        }
    }

    async #sendChangesImpl(config, prop, value, oldValue) {
        if (prop.__busy) {
            return FunctionUtils.ThrottleDelay;
        }

        const {control} = this.propertyMeta[prop.key];

        const dataSavingMode = !["wheel", "color"].includes(prop.type);

        prop.__busy = true;
        try {
            if (dataSavingMode) control.setAttribute("data-saving", "true");

            if (Array.isArray(prop.cmd)) {
                await this.#ws.request(value ? prop.cmd[0] : prop.cmd[1]);
            } else if (prop.type === "text" || prop.type === "password") {
                const str = prop.maxLength ? value.slice(0, prop.maxLength) : value;
                let buffer = new TextEncoder().encode(str).buffer;

                if (prop.maxLength && prop.kind === "FixedString") {
                    const paddedBuffer = new Uint8Array(prop.maxLength).fill(0);
                    // Need to slice twice, because some symbols takes more byte space (like emoji)
                    const dataBuffer = new Uint8Array(buffer).slice(0, prop.maxLength);
                    paddedBuffer.set(dataBuffer);
                    buffer = paddedBuffer.buffer;
                }

                await this.#ws.request(prop.cmd, buffer);
                control.setValue(new BinaryParser(buffer).readFixedString(prop.maxLength));
            } else {
                const kind = prop.kind ?? "Uint8";
                const size = {
                    Boolean: 1,
                    Uint8: 1, Int8: 1,
                    Uint16: 2, Int16: 2,
                    Uint32: 4, Int32: 4,
                    BigUint64: 8, BigInt64: 8,
                    Float32: 4,
                    Float64: 8,
                }[kind];

                if (typeof size === "undefined") {
                    // noinspection ExceptionCaughtLocallyJS
                    throw new Error(`Unknown kind: "${kind}"`);
                }

                const serializeFn = {
                    Boolean: DataView.prototype.setUint8,
                    Uint8: DataView.prototype.setUint8,
                    Int8: DataView.prototype.setInt8,
                    Uint16: DataView.prototype.setUint16,
                    Int16: DataView.prototype.setInt16,
                    Uint32: DataView.prototype.setUint32,
                    Int32: DataView.prototype.setInt32,
                    BigUint64: function (offset, value, littleEndian) {
                        this.setBigUint64(0, BigInt(value), littleEndian);
                    },
                    BigInt64: function (offset, value, littleEndian) {
                        this.setBigInt64(0, BigInt(value), littleEndian);
                    },
                    Float32: DataView.prototype.setFloat32,
                    Float64: DataView.prototype.setFloat64,
                }[kind];

                const req = new Uint8Array(size);
                const view = new DataView(req.buffer);
                serializeFn.call(view, 0, value, true);

                await this.#ws.request(prop.cmd, req.buffer);
                if (dataSavingMode) control.setValue(value);
            }

            console.log(`Changed '${prop.key}': '${oldValue}' -> '${value}'`);
        } catch (e) {
            console.error("Unable to save changes", e);
            control.setValue(oldValue);
        } finally {
            prop.__busy = false;
            if (dataSavingMode) control.setAttribute("data-saving", "false");
        }
    }
}