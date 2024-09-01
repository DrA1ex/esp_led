import {AppConfigBase} from "./lib/index.js";

import {PropertyConfig} from "./props.js";
import {PacketType} from "./cmd.js";


export class Config extends AppConfigBase {
    power;
    rgb_mode;
    brightness;
    color;
    calibration
    nightMode;

    constructor() {
        super(PropertyConfig);
    }

    get cmd() {return PacketType.GET_CONFIG;}

    parse(parser) {
        this.power = parser.readBoolean();
        this.rgb_mode = parser.readBoolean();

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
    }
}