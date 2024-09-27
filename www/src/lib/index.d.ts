// misc/event_emitter.js
declare module "misc/event_emitter.js" {
    export class EventEmitter {
        /**
         * Emits an event of a given type with optional data.
         * @param type - The type of the event.
         * @param data - Optional data to be passed to the event handlers.
         */
        emitEvent(type: string, data?: any | null): void;

        /**
         * Subscribes a handler to a specific event type for the given subscriber.
         * @param subscriber - The object subscribing to the event.
         * @param type - The type of the event.
         * @param handler - The function to be called when the event is emitted.
         */
        subscribe(subscriber: object, type: string, handler: (emitter: EventEmitter, data: any | null) => void): void;

        /**
         * Unsubscribes a handler from a specific event type for the given subscriber.
         * @param subscriber - The object unsubscribing from the event.
         * @param type - The type of the event.
         */
        unsubscribe(subscriber: object, type: string): void;
    }
}

declare module "misc/binary_parser.js" {
    export class BinaryParser {
        get position(): number;

        /**
         * Construct a BinaryParser instance
         *
         * @param buffer - The buffer to parse
         * @param byteOffset - The byte offset within the buffer, defaults to 0
         */
        constructor(buffer: ArrayBuffer, byteOffset?: number);

        /**
         * Read a string from the buffer
         * @param byteLength - The parsed string length
         * @param encoding - The parsed string encoding
         * @returns The parsed string value
         */
        readString(byteLength: number, encoding?: string): string;

        /**
         * Read a null-terminated string from the buffer
         * @param encoding - The parsed string encoding
         * @returns The parsed string value
         */
        readNullTerminatedString(encoding?: string): string;

        /**
         * Read a null-terminated string with fixed size from the buffer
         * @param byteLength - Size of string field in bytes
         * @param encoding - The parsed string encoding
         * @returns The parsed string value
         */
        readFixedString(byteLength: number, encoding?: string): string;

        /**
         * Read a boolean from the buffer
         * @returns The parsed boolean value
         */
        readBoolean(): boolean;

        /**
         * Read an unsigned 8-bit integer from the buffer
         * @returns The parsed 8-bit unsigned integer value
         */
        readUint8(): number;

        /**
         * Read a signed 8-bit integer from the buffer
         * @returns The parsed 8-bit signed integer value
         */
        readInt8(): number;

        /**
         * Read an unsigned 16-bit integer from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 16-bit unsigned integer value
         */
        readUint16(littleEndian?: boolean): number;

        /**
         * Read a signed 16-bit integer from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 16-bit signed integer value
         */
        readInt16(littleEndian?: boolean): number;

        /**
         * Read an unsigned 32-bit integer from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 32-bit unsigned integer value
         */
        readUint32(littleEndian?: boolean): number;

        /**
         * Read a signed 32-bit integer from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 32-bit signed integer value
         */
        readInt32(littleEndian?: boolean): number;

        /**
         * Read a 32-bit floating point number from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 32-bit floating point number
         */
        readFloat32(littleEndian?: boolean): number;

        /**
         * Read a 64-bit floating point number from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 64-bit floating point number
         */
        readFloat64(littleEndian?: boolean): number;

        /**
         * Read a 64-bit unsigned integer from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 64-bit unsigned integer value as a BigInt
         */
        readBigUint64(littleEndian?: boolean): BigInteger;

        /**
         * Read a 64-bit signed integer from the buffer
         * @param littleEndian - Specify byte order (little or big endian)
         * @returns The parsed 64-bit signed integer value as a BigInt
         */
        readBigInt64(littleEndian?: boolean): BigInteger;
    }
}

// network/ws.js
declare module "network/ws.js" {
    import {EventEmitter} from "misc/event_emitter.js";
    import {BinaryParser} from "misc/binary_parser.js";

    export enum WebSocketState {
        uninitialized = "uninitialized",
        disconnected = "disconnected",
        connecting = "connecting",
        reconnecting = "reconnecting",
        connected = "connected"
    }

    enum SystemPacketType {
        RESPONSE_STRING = 0xf0,
        RESPONSE_BINARY = 0xf1,
    }

    class Packet<PacketTypeT> {
        requestId: number;
        type: PacketTypeT | SystemPacketType;
        signature: number;
        size: number;
        data: Uint8Array;

        parseString(): string;
        parser(): BinaryParser;

        static parse<PacketTypeT>(buffer: ArrayBuffer): Packet<PacketTypeT>;
    }

    export class WebSocketConfig {
        /**
         * The timeout for requests in milliseconds.
         * @default 2000
         */
        requestTimeout: number;

        /**
         * The step increase for connection timeout delay in milliseconds.
         * @default 500
         */
        connectionTimeoutDelayStep: number;

        /**
         * The maximum connection timeout delay in milliseconds.
         * @default 5000
         */
        connectionTimeoutMaxDelay: number;

        /**
         * The timeout for closing the connection in milliseconds.
         * @default 500
         */
        connectionCloseTimeout: number;

        /**
         * An array representing the request signature.
         * @default [0xba, 0xda]
         */
        requestSignature: number[];
    }

    export class WebSocketInteraction extends EventEmitter {
        static Event: {
            Connected: "ws_interaction_connected";
            Disconnected: "ws_interaction_disconnected";
            Error: "ws_interaction_error";
            Message: "ws_interaction_message";
            Notification: "ws_interaction_notification";
        };

        readonly gateway: string;
        readonly requestTimeout: number;

        readonly connected: boolean;
        readonly state: WebSocketState;

        constructor(gateway: string, config?: WebSocketConfig);

        begin(): void;
        connect(): void;
        close(): void;

        /**
         * @param cmd - Command type (generic)
         * @param buffer - Optional payload as ArrayBuffer
         * @returns Promise that resolves with Packet
         */
        request<CmdEnumT>(cmd: CmdEnumT, buffer?: ArrayBuffer | null): Promise<Packet<SystemPacketType>>;
    }
}

// control/text.js
declare module "control/index.js" {
    export class Control {
        constructor(element: HTMLElement);

        setVisibility(show: boolean): void;
        setEnabled(enabled: boolean): void;
        setTooltip(text: string): void;
        setInteractions(enable: boolean): void;
        addClass(className: string): void;
        removeClass(className: string): void;
        setAttribute(attribName: string, value: any): void;
        getAttribute(attribName: string): string | null;

        static byId(id: string, ...params: any[]): HTMLElement;
    }

    export abstract class InputControlBase<T> extends Control {
        protected constructor(element: HTMLInputElement);

        abstract getValue(): T;

        setValue(value: any): void;
        setOnChange(fn: (value: any, prevValue: any) => void): void;
        protected _emitChanged(value: any, prevValue: any): void;
    }

    export class ButtonControl extends Control {
        constructor(element: HTMLElement);

        setLabel(label: string): void;
        setOnClick(fn: (control: ButtonControl, event: MouseEvent) => void): void;
    }

    export class FrameControl extends Control {
        constructor(element: HTMLElement);

        appendChild(control: Control): void;
    }

    export enum InputType {
        text = "text",
        password = "password",
        int = "int",
        float = "float",
        time = "time",
        color = "color",
    }

    export class InputControl extends InputControlBase<string | number> {
        constructor(element: HTMLInputElement, type?: InputType);

        getValue(): string | number | null;
        setValue(value: any): void;
        isValid(): boolean;
        setMaxLength(maxlength: number): void;
    }

    export class SelectControl extends InputControlBase<string> {
        options: Array<{ key: string; label: string }>;

        constructor(element: HTMLElement);

        setOptions(options: Array<{ key: string; label: string } | string>): void;
        updateOption(key: string, label: string): void;
        select(key: string): void;
        getValue(): string | null;
    }

    export class TextControl extends Control {
        constructor(element: HTMLElement);

        get text(): string;
        setText(text: string): void;
    }

    export class TriggerControl extends InputControlBase<boolean> {
        constructor(element: HTMLElement);

        setValue(flag: boolean): void;
        getValue(): boolean;
    }

    export class WheelControl extends InputControlBase<number> {
        margin: number;
        limit: number;

        constructor(element: HTMLElement, limit: number);

        setValue(value: number): void;
        getValue(): number;
        setDisplayConverter(fn: (value: number) => string | Array<string>): void;
    }
}

// main module
declare module "application.js" {
    import {Control, FrameControl, TextControl} from "control/index.js";
    import {WebSocketInteraction, WebSocketConfig} from "network/ws.js";
    import {EventEmitter} from "misc/event_emitter.js";
    import {BinaryParser} from "misc/binary_parser.js";

    export enum ControlType {
        trigger = "trigger",
        wheel = "wheel",
        time = "time",
        select = "select",
        int = "int",
        text = "text",
        password = "password",
        color = "color",
        button = "button",
        skip = "skip",
        title = "title",
        label = "label",
        separator = "separator",
    }

    export enum PropertyKind {
        Boolean = "Boolean",
        Uint8 = "Uint8",
        Int8 = "Int8",
        Uint16 = "Uint16",
        Int16 = "Int16",
        Uint32 = "Uint32",
        Int32 = "Int32",
        BigUint64 = "BigUint64",
        BigInt64 = "BigInt64",
        Float32 = "Float32",
        Float64 = "Float64",
        FixedString = "FixedString",
        NullTerminatedString = "NullTerminatedString",
    }

    export interface BaseConfig {
        key: string;
        type: string;
        title?: string;
        visibleIf?: string;
        extra?: { m_top?: boolean };
    }

    export interface CommandBaseConfig extends BaseConfig {
        cmd: [number, number] | number;
        kind: keyof typeof PropertyKind;
    }

    export interface TriggerConfig extends CommandBaseConfig {
        type: "trigger";
    }

    export interface WheelConfig extends CommandBaseConfig {
        type: "wheel";
        limit: number;
        min?: number;
        anchor?: number;
        anchorAmount?: number;
        anchored?: boolean;
        displayConverter: (value: number) => string;
    }

    export interface TimeConfig extends CommandBaseConfig {
        type: "time";
    }

    export interface SelectConfig extends CommandBaseConfig {
        type: "select";
        list: number;
    }

    export interface IntConfig extends CommandBaseConfig {
        type: "int";
        min?: number;
        limit?: number;
    }

    export interface TextConfig extends CommandBaseConfig {
        type: "text";
        maxLength: number;
    }

    export interface PasswordConfig extends CommandBaseConfig {
        type: "password";
        maxLength: number;
    }

    export interface ColorConfig extends CommandBaseConfig {
        type: "color";
    }

    export interface ButtonConfig extends CommandBaseConfig {
        type: "button";
        label: string;
    }

    export interface LabelConfig extends CommandBaseConfig {
        type: "label";
        label: string;
    }

    export interface SkipConfig {
        key: string,
        type: "skip";
    }

    export interface TitleConfig extends BaseConfig {
        type: "title";
        label: string;
    }

    export interface SeparatorConfig {
        type: "separator";
    }

// Union type
    export type PropertyConfig =
        | TriggerConfig
        | WheelConfig
        | TimeConfig
        | SelectConfig
        | IntConfig
        | TextConfig
        | PasswordConfig
        | ColorConfig
        | ButtonConfig
        | SkipConfig
        | TitleConfig
        | LabelConfig
        | SeparatorConfig;

    export interface SectionConfig {
        key: string;
        section: string;
        lock?: boolean;
        collapse?: boolean;
        visibleIf?: string;
        props: PropertyConfig[];
    }

    export type PropertiesConfig = SectionConfig[];

    export interface SelectOptionsConfig {
        code: number;
        name: string;
    }

    export type SelectListConfig = Record<string, SelectOptionsConfig>

    export interface PropertyMeta {
        prop: PropertyConfig;
        title: TextControl;
        control: Control;
    }

    export interface PropertySectionMeta {
        section: FrameControl;
        config: SectionConfig;
        props: PropertyMetadataMap;
    }

    export type PropertyMetadataMap = Record<string, PropertyMeta>;
    export type PropertySectionMap = Record<string, PropertySectionMeta>;
    export type PropertyConfigMap = Record<string, PropertyConfig>;

    export class ApplicationCfg {
        /**
         * @default 1000/60
         */
        throttleInterval: number;
        webSocketConfig: WebSocketConfig;
    }

    export abstract class AppConfigBase extends EventEmitter {
        static Event: {
            Loaded: string;
            PropertyChanged: string;
        };

        abstract get cmd(): number;
        abstract parse(parser: BinaryParser): void;

        protected constructor(propertyConfig: PropertiesConfig);

        get propertyMap(): PropertyConfigMap;

        load(ws: WebSocketInteraction): Promise<void>;
        getProperty<T>(key: string): T;
        setProperty<T>(key: string, value: T, sendNotification?: boolean): void;
    }

    export abstract class ApplicationBase extends EventEmitter {
        abstract initialize(): Promise<void>;
        abstract refresh(): Promise<void>;
        abstract get propertyConfig(): PropertiesConfig;
        abstract get config(): AppConfigBase;

        readonly ws: WebSocketInteraction;

        readonly propertyMeta: PropertyMetadataMap;
        readonly propertySections: PropertySectionMap;

        protected constructor(wsUrl: string, config?: ApplicationCfg);

        begin(root: HTMLElement): Promise<void>;

        sendChanges: (config: any, prop: PropertyConfig, value: number, oldValue: any) => Promise<void>;
    }
}
