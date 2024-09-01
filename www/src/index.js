import {Config} from "./config.js";
import {Properties, PropertyConfig} from "./props.js";
import {CONNECTION_TIMEOUT_DELAY_STEP, DEFAULT_ADDRESS, THROTTLE_INTERVAL} from "./constants.js";


import {TriggerControl} from "./lib/control/trigger.js";
import {FrameControl} from "./lib/control/frame.js";
import {TextControl} from "./lib/control/text.js";
import {WheelControl} from "./lib/control/wheel.js";
import {SelectControl} from "./lib/control/select.js";
import {ButtonControl} from "./lib/control/button.js";
import {InputControl, InputType} from "./lib/control/input.js";

import {BinaryParser} from "./lib/misc/binary_parser.js";
import {WebSocketInteraction} from "./lib/network/ws.js";

import * as FunctionUtils from "./lib/utils/function.js"

if ("serviceWorker" in navigator) {
    try {
        const registration = await navigator.serviceWorker.register("./service_worker.js");
        console.log("ServiceWorker registration successful with scope: ", registration.scope);
    } catch (e) {
        console.log("ServiceWorker registration failed: ", e);
    }
}

const StatusElement = document.getElementById("status");

const params = new Proxy(new URLSearchParams(window.location.search), {
    get: (searchParams, prop) => searchParams.get(prop),
});

const protocol = window.location.protocol === "https:" ? "wss" : "ws";
const host = params.host ?? window.location.hostname;
const gateway = `${protocol}://${host !== "localhost" ? host : DEFAULT_ADDRESS}/ws`;

window.__app = {};
initUi();

const ws = new WebSocketInteraction(gateway);
let initialized = false;

ws.subscribe(this, WebSocketInteraction.Event.Connected, async () => {
    try {
        if (!initialized) {
            await initialize();
            initialized = true;
        } else {
            await refresh();
        }

        StatusElement.style.visibility = "collapse";
    } catch (e) {
        console.error("Unable to finish initialization after connection established.", e);

        ws.close();
        setTimeout(() => ws.connect(), CONNECTION_TIMEOUT_DELAY_STEP);
    }
});

ws.subscribe(this, WebSocketInteraction.Event.Disconnected, () => {
    StatusElement.innerText = "NOT CONNECTED";
    StatusElement.style.visibility = "visible";
});

ws.subscribe(this, WebSocketInteraction.Event.Notification, (_, packet) => {
    const property = Object.values(window.__app.Properties)
        .find(p => p.prop.cmd instanceof Array ? p.prop.cmd.includes(packet.type) : p.prop.cmd === packet.type);

    if (!property) return console.error("Received notification for unknown property", packet.type);

    if (property.prop.cmd instanceof Array) {
        window.__app.Config.setProperty(property.prop.key, property.prop.cmd[0] === packet.type, false);
    } else {
        window.__app.Config.setProperty(property.prop.key, packet.parser()[`read${property.prop.kind}`](), false);
    }

    property.control.setValue(window.__app.Config.getProperty(property.prop.key));
});

function startSection(title, lock) {
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
    }

    const sectionTitle = new TextControl(document.createElement("h3"));
    sectionTitle.addClass("section-title");
    sectionTitle.setText(title);

    frame.appendChild(sectionTitle);
    document.body.appendChild(frame.element);

    return frame;
}

function createTitle(title) {
    const titleElement = new TextControl(document.createElement("p"));
    titleElement.setText(title);

    return titleElement;
}

function initUi() {
    const Sections = {};
    const Properties = {};

    for (const cfg of PropertyConfig) {
        const section = startSection(cfg.section, cfg.lock ?? false);

        Sections[cfg.key] = {section, cfg, props: {}};

        for (const prop of cfg.props) {
            let title = null
            if (prop.title) {
                title = createTitle(prop.title);
                section.appendChild(title);
            }

            let control = null;
            switch (prop.type) {
                case "trigger":
                    control = new TriggerControl(document.createElement("a"));
                    break;

                case "wheel":
                    control = new WheelControl(document.createElement("div"), prop.limit);
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

                case "text":
                    control = new InputControl(document.createElement("input"), InputType.text);
                    control.setMaxLength(prop.maxLength ?? 255);
                    break;

                case "color":
                    control = new InputControl(document.createElement("input"), InputType.color);
                    break;

                case "button":
                    control = new ButtonControl(document.createElement("a"));
                    control.addClass("m-top");
                    control.setLabel(prop.label);
                    break;

                case "skip":
                    break;

                default:
                    console.error("Invalid prop type.", prop)
            }

            if (control) {
                control.setAttribute("data-loading", true);
                if (prop.displayConverter) control.setDisplayConverter(prop.displayConverter);

                section.appendChild(control);
            }

            const entry = {prop, title, control};
            Sections[cfg.key].props[prop.key] = entry;
            Properties[prop.key] = entry;
        }
    }

    window.__app.Sections = Sections;
    window.__app.Properties = Properties;
}

async function initialize() {
    const config = new Config(ws);

    window.__app.Config = config;

    await config.load();

    const Lists = {};

    console.log("Config", config);
    console.log("Lists", Lists);

    window.__app.Lists = Lists;

    config.subscribe(this, Config.Event.Loaded, refreshConfig);
    config.subscribe(this, Config.Event.PropertyChanged, onConfigPropChanged);

    refreshConfig();
}

function refreshConfig() {
    const config = window.__app.Config;
    const lists = window.__app.Lists;

    for (const cfg of PropertyConfig) {
        const section = window.__app.Sections[cfg.key];
        for (const prop of cfg.props) {
            if (!prop.key || prop.type === "skip") continue;

            const {control, title} = section.props[prop.key];

            if (prop.visibleIf) {
                if (config.getProperty(prop.visibleIf)) {
                    control.setVisibility(true);
                    title.setVisibility(true);
                } else {
                    control.setVisibility(false);
                    title.setVisibility(false);
                    continue;
                }
            }

            if (prop.type === "select") {
                control.setOptions(lists[prop.list].map(v => ({key: v.code, label: v.name})));
            }

            if (prop.type !== "button") {
                const value = config.getProperty(prop.key);
                control.setValue(value);
            }

            if (control.getAttribute("data-loading") === "true") {
                if ("setOnChange" in control) control.setOnChange((value) => config.setProperty(prop.key, value));

                control.setAttribute("data-loading", false);
            }
        }
    }
}

async function onConfigPropChanged(config, {key, value, oldValue}) {
    const prop = Properties[key];
    if (!prop) {
        console.error(`Unknown property ${key}`);
        return;
    }

    if (typeof prop.min !== "undefined" && value < prop.min) value = prop.min;
    if (typeof prop.max !== "undefined" && value > prop.max) value = prop.max;

    if (value !== oldValue) {
        await sendChanges(config, prop, value, oldValue);
    }
}

const sendChanges = FunctionUtils.throttle(async function (config, prop, value, oldValue) {
    if (prop.__busy) {
        return FunctionUtils.ThrottleDelay;
    }

    const control = window.__app.Properties[prop.key].control;
    prop.__busy = true;
    try {
        if (prop.type !== "wheel") control.setAttribute("data-saving", true);

        if (Array.isArray(prop.cmd)) {
            await ws.request(value ? prop.cmd[0] : prop.cmd[1]);
        } else if (prop.type === "text") {
            await ws.request(prop.cmd, new TextEncoder().encode(value).buffer);
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
                BigUint64: DataView.prototype.setBigUint64,
                BigInt64: DataView.prototype.setBigInt64,
                Float32: DataView.prototype.setFloat32,
                Float64: DataView.prototype.setFloat64,
            }[kind];

            const req = new Uint8Array(size);
            const view = new DataView(req.buffer);
            serializeFn.call(view, 0, value, true);

            await ws.request(prop.cmd, req.buffer);
        }

        console.log(
            `Changed '${prop.key}': '${oldValue}' -> '${value}'`
        );
    } catch (e) {
        console.error("Unable to save changes", e);
        control.setValue(oldValue);
    } finally {
        prop.__busy = false;
        if (prop.type !== "wheel") control.setAttribute("data-saving", false);
    }
}, THROTTLE_INTERVAL);

async function refresh() {
    const config = window.__app.Config;
    await config.load();

    console.log("New config", config);
}

ws.begin();
window.__ws = ws;

document.addEventListener("visibilitychange", (e) => {
    console.log("Hidden", document.hidden);

    if (document.hidden) {
        ws.close();
    } else {
        ws.connect();
    }
});