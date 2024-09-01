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

export class Application extends ApplicationBase {
    #config;

    get propertyConfig() {return PropertyConfig;}

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
}