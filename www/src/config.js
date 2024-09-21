import {AppConfigBase} from "./lib/index.js";

import {PropertyConfig} from "./props.js";
import {PacketType} from "./cmd.js";


export class Config extends AppConfigBase {
    power;
    brightness;
    color;
    calibration
    nightMode;

    sysConfig;

    rgbMode;
    singleLedMode

    constructor() {
        super(PropertyConfig);

        this.lists["wifiMode"] = [
            {code: 0, name: "AP"},
            {code: 1, name: "STA"},
        ];
    }

    get cmd() {return PacketType.GET_CONFIG;}

    parse(parser) {
        this.power = parser.readBoolean();

        this.brightness = parser.readUint16();

        this.color = parser.readUint32();
        this.calibration = parser.readUint32();

        this.nightMode = {
            enabled: parser.readBoolean(),
            brightness: parser.readUint16(),

            startTime: parser.readUint32(),
            endTime: parser.readUint32(),
            switchInterval: parser.readUint16(),
        };

        this.sysConfig = {
            mdnsName: parser.readFixedString(32),

            wifiMode: parser.readUint8(),
            wifiSsid: parser.readFixedString(32),
            wifiPassword: parser.readFixedString(32),

            wifiConnectionCheckInterval: parser.readUint32(),
            wifiMaxConnectionAttemptInterval: parser.readUint32(),

            rgbMode: this.rgbMode = parser.readBoolean(),

            ledRPin: parser.readUint8(),
            ledGPin: parser.readUint8(),
            ledBPin: parser.readUint8(),

            ledMinBrightness: parser.readUint16(),
            powerChangeTimeout: parser.readUint32(),
            wifiConnectFlashTimeout: parser.readUint32(),

            timeZone: parser.readFloat32(),

            webAuth: parser.readBoolean(),
            webAuthUser: parser.readFixedString(32),
            webAuthPassword: parser.readFixedString(32),

            mqtt: parser.readBoolean(),
            mqttHost: parser.readFixedString(32),
            mqttPort: parser.readUint16(),
            mqttUser: parser.readFixedString(32),
            mqttPassword: parser.readFixedString(32),
            mqttConvertBrightness: parser.readBoolean(),
        };

        this.sysConfig.ledPin = this.sysConfig.ledRPin;
        this.singleLedMode = !this.rgbMode;
    }
}