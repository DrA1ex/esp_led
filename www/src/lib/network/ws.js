import {SystemPacketType} from "./cmd.js";
import {Packet} from "./packet.js";
import {EventEmitter} from "../misc/event_emitter.js";

/**
 * @enum {string}
 */
export const WebSocketState = {
    uninitialized: "uninitialized",
    disconnected: "disconnected",
    connecting: "connecting",
    reconnecting: "reconnecting",
    connected: "connected"
}

const MAX_REQUEST_ID = 2 ** 16 - 1;

export class WebSocketConfig {
    /**
     * The timeout for requests in milliseconds.
     * @type {number}
     * @default 2000
     */
    requestTimeout = 2000;

    /**
     * The step increase for connection timeout delay in milliseconds.
     * @type {number}
     * @default 500
     */
    connectionTimeoutDelayStep = 500;

    /**
     * The maximum connection timeout delay in milliseconds.
     * @type {number}
     * @default 5000
     */
    connectionTimeoutMaxDelay = 5000;

    /**
     * The timeout for closing the connection in milliseconds.
     * @type {number}
     * @default 500
     */
    connectionCloseTimeout = 500;

    /**
     * An array representing the request signature.
     * @type {number[]}
     * @default [0xba, 0xda]
     */
    requestSignature = [0xba, 0xda];
}

export class WebSocketInteraction extends EventEmitter {
    static Event = {
        Connected: "ws_interaction_connected",
        Disconnected: "ws_interaction_disconnected",
        Error: "ws_interaction_error",
        Message: "ws_interaction_message",
        Notification: "ws_interaction_notification",
    }

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

    get connected() {return this.#state === WebSocketState.connected;}
    get state() {return this.#state;}

    constructor(gateway, config = null) {
        super();

        this.gateway = gateway;
        this.config = Object.assign(new WebSocketConfig(), config || {});
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
     * @param {CmdEnumT} cmd
     * @param {ArrayBuffer} [buffer=null]
     * @returns {Promise<Packet>}
     *
     * @template CmdEnumT
     */
    async request(cmd, buffer = null) {
        if (!this.connected) {
            throw new Error("Not connected");
        }

        const requestId = this.#getRequestId();
        const requestIdBytes = new Uint8Array(Uint16Array.of(requestId).buffer);

        if (buffer) {
            if (buffer.byteLength > 255) throw new Error("Request payload too long!");

            const requestLengthBytes = new Uint8Array(Uint16Array.of(buffer.byteLength).buffer);
            this.#ws.send(Uint8Array.of(...this.config.requestSignature, ...requestIdBytes, cmd, ...requestLengthBytes, ...new Uint8Array(buffer)));
        } else {
            this.#ws.send(Uint8Array.of(...this.config.requestSignature, ...requestIdBytes, cmd, 0x00, 0x00));
        }

        return new Promise((resolve, reject) => {
            const timer = setTimeout(() => {
                this.#closeConnection();
                reject(new Error("Request timeout"));
            }, this.config.requestTimeout);

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

        this.emitEvent(WebSocketInteraction.Event.Connected);
    }

    #onError(e) {
        console.error(`#${this.#ws.__id}: WebSocket error`, e);

        this.emitEvent(WebSocketInteraction.Event.Error, e);
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

        this.emitEvent(WebSocketInteraction.Event.Message, e);

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

                return this.emitEvent(WebSocketInteraction.Event.Notification, packet);
            } else if (!(packet.requestId in this.#requestMap)) {
                return console.error(`Websocket unknown requestId: ${packet.requestId}`, packet);
            }
        } catch (e) {
            console.error("Websocket unable to parse message", e);
            return;
        }

        const request = this.#requestMap[packet.requestId];

        try {
            if (packet.type === SystemPacketType.RESPONSE_STRING) {
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

        this.emitEvent(WebSocketInteraction.Event.Disconnected);
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
            }, this.config.connectionCloseTimeout);
        }

        console.log(`#${this.#ws.__id}: WebSocket connection closed`);
        this.#ws = null;
    }

    #reconnect() {
        if (this.#state === WebSocketState.reconnecting) return;

        this.#reconnectionTimerId = setTimeout(() => this.#init(), this.#reconnectionTimeout);
        console.log("WebSocket: try reconnect after", this.#reconnectionTimeout);

        this.#state = WebSocketState.reconnecting;
        this.#reconnectionTimeout = Math.min(this.config.connectionTimeoutMaxDelay,
            this.#reconnectionTimeout + this.config.connectionTimeoutDelayStep);
    }

    #clearConnectionTimerIfNeeded() {
        if (this.#state === WebSocketState.reconnecting) {
            clearTimeout(this.#reconnectionTimerId);

            this.#reconnectionTimerId = null;
            this.#state = WebSocketState.disconnected;
        }
    }
}