import {BinaryParser} from "../misc/binary_parser.js";

export class Packet {
    signature;
    requestId;
    type;
    size;
    data;

    static parse(buffer) {
        const parser = new BinaryParser(buffer);

        const packet = new Packet();

        packet.signature = parser.readUInt16();
        packet.requestId = parser.readUInt16();
        packet.type = parser.readUInt8();
        packet.size = parser.readUInt8();
        packet.data = new Uint8Array(buffer, parser.position);

        return packet;
    }

    parser() {
        return new BinaryParser(this.data.buffer, this.data.byteOffset);
    }

    parseString() {
        return this.parser().readString(this.data.size);
    }
}