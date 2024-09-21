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

        packet.signature = parser.readUint16();
        packet.requestId = parser.readUint16();
        packet.type = parser.readUint8();
        packet.size = parser.readUint16();
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