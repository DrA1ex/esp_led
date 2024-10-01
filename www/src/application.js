import {AppConfigBase, ApplicationBase} from "./lib/index.js";
import {Config} from "./config.js";
import {PropertyConfig} from "./props.js";

import {
    CONNECTION_CLOSE_TIMEOUT,
    CONNECTION_TIMEOUT_DELAY_STEP,
    CONNECTION_TIMEOUT_MAX_DELAY,
    REQUEST_SIGNATURE,
    REQUEST_TIMEOUT,
    THROTTLE_INTERVAL
} from "./constants.js";

import {PacketType} from "./cmd.js";

export class Application extends ApplicationBase {
    #config;
    #reHost = /([?&]host=)(.*)(?:$|&)/;

    get propertyConfig() {return PropertyConfig;}

    /**
     * @returns {Config}
     */
    get config() {return this.#config;}

    constructor(wsUrl) {
        super(wsUrl, {
            throttleInterval: THROTTLE_INTERVAL,
            webSocketConfig: {
                requestTimeout: REQUEST_TIMEOUT,
                requestSignature: REQUEST_SIGNATURE,
                connectionTimeoutDelayStep: CONNECTION_TIMEOUT_DELAY_STEP,
                connectionTimeoutMaxDelay: CONNECTION_TIMEOUT_MAX_DELAY,
                connectionCloseTimeout: CONNECTION_CLOSE_TIMEOUT,
            }
        });

        this.#config = new Config();
    }


    async begin(root) {
        await super.begin(root);

        this.propertyMeta["apply_sys_config"].control.setOnClick(this.applySysConfig.bind(this));
        this.propertyMeta["apply_led_config"].control.setOnClick(this.applySysConfig.bind(this));

        this.subscribe(this, this.Event.PropertyCommited, this.onPropCommited.bind(this));
    }

    async applySysConfig(sender) {
        if (sender.getAttribute("data-saving") === "true") return;

        sender.setAttribute("data-saving", true);

        try {
            await this.ws.request(PacketType.RESTART);

            const newHostname = this.config.sysConfig.mdnsName + ".local";
            const hostQueryMatch = location.search.match(this.#reHost);
            if (hostQueryMatch && hostQueryMatch[2] !== newHostname) {
                const new_url = location.search.replace(this.#reHost, `$1${newHostname}`)
                setTimeout(() => window.location = new_url, 3000);
            } else if (!hostQueryMatch && location.hostname !== "localhost" && location.hostname !== newHostname) {
                const url_parts = [
                    location.protocol + "//",
                    newHostname,
                    location.port ? ":" + location.port : "",
                    "/?" + (location.href.split("?")[1] ?? "")
                ]

                const new_url = url_parts.join("");
                setTimeout(() => window.location = new_url, 3000);
            } else {
                setTimeout(() => window.location.reload(), 3000);
            }
        } catch (err) {
            console.log("Unable to send restart signal", err);
            sender.setAttribute("data-saving", false);
        }
    }

    onPropCommited(config, {key}) {
        if (key === "ledType") {
            this.config.refreshLedMode();

            for (const propKey of [
                "sysConfig.ledRPin",
                "sysConfig.ledGPin",
                "sysConfig.ledBPin",
                "sysConfig.ledWPin",
                "sysConfig.ledCPin",
                "sysConfig.ledPin"
            ]) {
                this.emitEvent(this.Event.Notification, {key: propKey, value: this.config.getProperty(propKey)});
            }
        }
    }
}