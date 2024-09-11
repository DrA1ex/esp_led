import {ApplicationBase} from "./lib/index.js";
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
    }

    async applySysConfig(sender) {
        if (sender.getAttribute("data-saving") === "true") return;

        sender.setAttribute("data-saving", true);

        try {
            await this.ws.request(PacketType.RESTART);

            let new_url
            if (location.hostname !== "localhost") {
                const url_parts = [
                    location.protocol + "//",
                    this.config.sysConfig.mdnsName + ".local",
                    location.port ? ":" + location.port : "",
                    "/?" + (location.href.split("?")[1] ?? "")
                ]

                new_url = url_parts.join("");
            } else {
                new_url = location.href;
            }

            setTimeout(() => window.location = new_url, 3000);
        } catch (err) {
            console.log("Unable to send restart signal", err);
            sender.setAttribute("data-saving", false);
        }
    }
}