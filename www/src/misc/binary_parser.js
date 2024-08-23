export class BinaryParser {
    #buffer;
    #byteOffset;

    #dataView;

    /**
     * Construct a BinaryParser instance
     *
     * @param {ArrayBuffer} buffer - The buffer to parse
     * @param {number} [byteOffset=0] - The byte offset within the buffer, defaults to 0
     */
    constructor(buffer, byteOffset = 0) {
        this.#buffer = buffer;
        this.#byteOffset = byteOffset;

        this.#dataView = new DataView(buffer);
    }

    /**
     * Read a string from the buffer
     * @param {number} byteLength - The parsed string length
     * @param {string} [encoding="utf8"] - The parsed string encoding
     * @returns {string} - The parsed string value
     */
    readString(byteLength, encoding = "utf8") {
        const strSlice = new Uint8Array(this.#buffer, this.#byteOffset, byteLength);
        this.#byteOffset += byteLength;

        return new TextDecoder(encoding).decode(strSlice);
    }

    /**
     * Read a null-terminated string from the buffer
     * @param {string} [encoding="utf8"] - The parsed string encoding
     * @returns {string} - The parsed string value
     */
    readNullTerminatedString(encoding = "utf8") {
        let length = 0;
        while (this.#dataView.getUint8(this.#byteOffset + length) !== 0) {
            ++length;
        }

        const str = this.readString(length, encoding);
        this.#byteOffset++; //Skip null-terminator

        return str;
    }

    /**
     * Read a null-terminated string with fixed size from the buffer
     * @param {number} byteLength - Size of string filed in bytes
     * @param {string} [encoding="utf8"] - The parsed string encoding
     * @returns {string} - The parsed string value
     */
    readFixedString(byteLength, encoding = "utf8") {
        let length = 0;
        for (let i = 0; i < byteLength; i++) {
            if (this.#dataView.getUint8(this.#byteOffset + length) === 0) {
                break;
            }

            ++length;
        }

        const str = this.readString(length, encoding);
        this.#byteOffset += byteLength - length;

        return str;
    }

    /**
     * Read a boolean from the buffer
     * @returns {boolean} - The parsed boolean value
     */
    readBoolean() {
        return this.readUInt8() === 1;
    }

    /**
     * Read an unsigned 8-bit integer from the buffer
     * @returns {number} - The parsed 8-bit unsigned integer value
     */
    readUInt8() {
        return this.#read(DataView.prototype.getUint8, 1);
    }

    /**
     * Read a signed 8-bit integer from the buffer
     * @returns {number} - The parsed 8-bit signed integer value
     */
    readInt8() {
        return this.#read(DataView.prototype.getInt8, 1);
    }

    /**
     * Read an unsigned 16-bit integer from the buffer
     * @returns {number} - The parsed 16-bit unsigned integer value
     */
    readUInt16(littleEndian = true) {
        return this.#read(DataView.prototype.getUint16, 2, littleEndian);
    }

    /**
     * Read a signed 16-bit integer from the buffer
     * @param {boolean} [littleEndian=true] - Specify byte order (little or big endian)
     * @returns {number} - The parsed 16-bit signed integer value
     */
    readInt16(littleEndian = true) {
        return this.#read(DataView.prototype.getInt16, 2, littleEndian);
    }

    /**
     * Read an unsigned 32-bit integer from the buffer
     * @param {boolean} [littleEndian=true] - Specify byte order (little or big endian)
     * @returns {number} - The parsed 32-bit unsigned integer value
     */
    readUInt32(littleEndian = true) {
        return this.#read(DataView.prototype.getUint32, 4, littleEndian);
    }

    /**
     * Read a signed 32-bit integer from the buffer
     * @param {boolean} [littleEndian=true] - Specify byte order (little or big endian)
     * @returns {number} - The parsed 32-bit signed integer value
     */
    readInt32(littleEndian = true) {
        return this.#read(DataView.prototype.getInt32, 4, littleEndian);
    }

    /**
     * Read a 32-bit floating point number from the buffer
     * @param {boolean} [littleEndian=true] - Specify byte order (little or big endian)
     * @returns {number} - The parsed 32-bit floating point number
     */
    readFloat32(littleEndian = true) {
        return this.#read(DataView.prototype.getFloat32, 4, littleEndian);
    }

    /**
     * Read a 64-bit floating point number from the buffer
     * @param {boolean} [littleEndian=true] - Specify byte order (little or big endian)
     * @returns {number} - The parsed 64-bit floating point number
     */
    readFloat64(littleEndian = true) {
        return this.#read(DataView.prototype.getFloat64, 8, littleEndian);
    }

    /**
     * Read a 64-bit unsigned integer from the buffer
     * @param {boolean} [littleEndian=true] - Specify byte order (little or big endian)
     * @returns {BigInt} - The parsed 64-bit unsigned integer value as a BigInt
     */
    readBigUint64(littleEndian = true) {
        return this.#read(DataView.prototype.getBigUint64, 8, littleEndian);
    }

    /**
     * Read a 64-bit signed integer from the buffer
     * @param {boolean} [littleEndian=true] - Specify byte order (little or big endian)
     * @returns {BigInt} - The parsed 64-bit signed integer value as a BigInt
     */
    readBigInt64(littleEndian = true) {
        return this.#read(DataView.prototype.getBigInt64, 8, littleEndian);
    }

    #read(method, size, littleEndian) {
        const value = method.call(this.#dataView, this.#byteOffset, littleEndian);
        this.#byteOffset += size;

        return value;
    }
}