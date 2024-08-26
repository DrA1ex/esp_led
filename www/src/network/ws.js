import {PacketType} from "./cmd.js";
import {Packet} from "./packet.js";
import {EventEmitter} from "../misc/event_emitter.js";

import {
    REQUEST_TIMEOUT,
    REQUEST_SIGNATURE,
    CONNECTION_TIMEOUT_DELAY_STEP,
    CONNECTION_CLOSE_TIMEOUT,
    CONNECTION_TIMEOUT_MAX_DELAY
} from "../constants.js";

/**
 * @enum {string}
 */
const WebSocketState = {
    uninitialized: "uninitialized",
    disconnected: "disconnected",
    connecting: "connecting",
    reconnecting: "reconnecting",
    connected: "connected"
}

const MAX_REQUEST_ID = 2 ** 16 - 1;

export class WebSocketInteraction extends EventEmitter {
    static CONNECTED = "ws_interaction_connected";
    static DISCONNECTED = "ws_interaction_disconnected";
    static ERROR = "ws_interaction_error";
    static MESSAGE = "ws_interaction_message";
    static NOTIFICATION = "ws_interaction_notification";

    #id = 0;
    #request_id = 1; // 0 - Reserved for notifications

    #getRequestId() {
        if (this.#request_id >= MAX_REQUEST_ID) this.#request_id = 1;
        return this.#request_id++;
    }

    #state = WebSocketState.uninitialized;
    #ws = null;

    #reconnectionTimeout = 0;
    #reconnectionTimerId = null;

    #requestMap = {};

    gateway;
    requestTimeout;

    get connected() {return this.#state === WebSocketState.connected;}
    get state() {return this.#state;}

    constructor(gateway, requestTimeout = REQUEST_TIMEOUT) {
        super();

        this.gateway = gateway;
        this.requestTimeout = requestTimeout;
    }

    begin() {
        if (this.#state !== WebSocketState.uninitialized) throw new Error("Already initialized");

        this.#state = WebSocketState.disconnected;
        this.connect();
    }

    connect() {
        if (this.#state === WebSocketState.uninitialized) throw new Error("Not initialized. Call begin() first.")

        if ([WebSocketState.disconnected, WebSocketState.reconnecting].includes(this.#state)) {
            this.#init();
        }
    }

    close() {
        if (this.#state === WebSocketState.uninitialized) throw new Error("Not initialized. Call begin() first.")

        if (this.#state !== WebSocketState.disconnected) {
            this.#closeConnection(false);
        }
    }

    /**
     * @param {PacketType} cmd
     * @param {ArrayBuffer} [buffer=null]
     * @returns {Promise<ArrayBuffer|*>}
     */
    async request(cmd, buffer = null) {
        if (!this.connected) {
            throw new Error("Not connected");
        }

        const requestId = this.#getRequestId();
        const requestIdBytes = new Uint8Array(Uint16Array.of(requestId).buffer);

        if (buffer) {
            if (buffer.byteLength > 255) throw new Error("Request payload too long!");

            this.#ws.send(Uint8Array.of(...REQUEST_SIGNATURE, ...requestIdBytes, cmd, buffer.byteLength, ...new Uint8Array(buffer)));
        } else {
            this.#ws.send(Uint8Array.of(...REQUEST_SIGNATURE, ...requestIdBytes, cmd, 0x00));
        }

        return new Promise((resolve, reject) => {
            const timer = setTimeout(() => {
                this.#closeConnection();
                reject(new Error("Request timeout"));
            }, this.requestTimeout);

            function _ok(...args) {
                clearTimeout(timer);
                resolve(...args);
            }

            function _fail(...args) {
                clearTimeout(timer);
                reject(...args);
            }

            function _abort() {
                clearTimeout(timer);
                reject(new Error("Request aborted"));
            }

            this.#requestMap[requestId] = {
                resolve: _ok, reject: _fail, abort: _abort
            };
        });
    }

    #init() {
        console.log("WebSocket connecting to", this.gateway);

        this.#clearConnectionTimerIfNeeded();

        this.#state = WebSocketState.connecting;
        this.#ws = new WebSocket(this.gateway);
        this.#ws.__id = this.#id++;

        this.#ws.onopen = this.#onOpen.bind(this);
        this.#ws.onclose = this.#onClose.bind(this);
        this.#ws.onerror = this.#onError.bind(this);
        this.#ws.onmessage = this.#onMessage.bind(this);
    }

    #onOpen() {
        this.#state = WebSocketState.connected;
        this.#reconnectionTimeout = 0;
        console.log(`#${this.#ws.__id}: WebSocket connection established`);

        this.emitEvent(WebSocketInteraction.CONNECTED);
    }

    #onError(e) {
        console.error(`#${this.#ws.__id}: WebSocket error`, e);

        this.emitEvent(WebSocketInteraction.ERROR, e);
        this.#closeConnection();
    }

    #onClose(e) {
        console.log(`${this.#ws.__id}: WebSocket close signal #${e.code}.`, e.reason);
        this.#closeConnection();
    }

    async #onMessage(e) {
        if (!this.connected) {
            console.error(`#${this.#ws.__id}: WebSocket unexpected message`, e);
            return;
        }

        this.emitEvent(WebSocketInteraction.MESSAGE, e);

        if (e.data instanceof Blob) {
            const buffer = await e.data.arrayBuffer()
            this.#processMessage(buffer);
        } else {
            console.log("WebSocket unsupported message type", e)
        }
    }

    #processMessage(buffer) {
        let packet;

        try {
            packet = Packet.parse(buffer);

            if (packet.requestId === 0) {
                console.log("Received notification of type", packet.type);

                return this.emitEvent(WebSocket.NOTIFICATION, packet);
            } else if (!(packet.requestId in this.#requestMap)) {
                return console.error(`Websocket unknown requestId: ${packet.requestId}`, packet);
            }
        } catch (e) {
            console.error("Websocket unable to parse message", e);
            return;
        }

        const request = this.#requestMap[packet.requestId];

        try {
            if (packet.type === PacketType.RESPONSE_STRING) {
                const str = packet.parseString();
                if (str !== "OK") return request.reject(str);
            }
        } catch (e) {
            console.error("Unable to read packet data", e);
            return request.reject(e);
        }

        request.resolve(packet);
    }

    #closeConnection(reconnect = true) {
        this.#cleanUpSocket();

        if (reconnect) {
            this.#reconnect();
        } else {
            this.#clearConnectionTimerIfNeeded();
            this.#state = WebSocketState.disconnected;
            this.#reconnectionTimeout = 0;
        }

        this.emitEvent(WebSocketInteraction.DISCONNECTED);
    }

    #cleanUpSocket() {
        const activeRequests = Object.values(this.#requestMap);
        if (activeRequests.length) {
            for (const entry of activeRequests) {
                entry.abort();
            }

            this.#requestMap = {};
        }

        if (!this.#ws) return;

        this.#ws.onopen = null;
        this.#ws.onclose = null;
        this.#ws.onerror = null;
        this.#ws.onmessage = null;

        if ([WebSocket.OPEN, WebSocket.CONNECTING].includes(this.#ws.readyState)) {
            const ws = this.#ws;

            const timerId = setInterval(() => {
                if (ws.readyState === WebSocket.CONNECTING) return;

                ws.close();
                clearInterval(timerId);
            }, CONNECTION_CLOSE_TIMEOUT);
        }

        console.log(`#${this.#ws.__id}: WebSocket connection closed`);
        this.#ws = null;
    }

    #reconnect() {
        if (this.#state === WebSocketState.reconnecting) return;

        this.#reconnectionTimerId = setTimeout(() => this.#init(), this.#reconnectionTimeout);
        console.log("WebSocket: try reconnect after", this.#reconnectionTimeout);

        this.#state = WebSocketState.reconnecting;
        this.#reconnectionTimeout = Math.min(CONNECTION_TIMEOUT_MAX_DELAY, this.#reconnectionTimeout + CONNECTION_TIMEOUT_DELAY_STEP);
    }

    #clearConnectionTimerIfNeeded() {
        if (this.#state === WebSocketState.reconnecting) {
            clearTimeout(this.#reconnectionTimerId);

            this.#reconnectionTimerId = null;
            this.#state = WebSocketState.disconnected;
        }
    }
}