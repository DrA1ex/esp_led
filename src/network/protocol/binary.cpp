#include "binary.h"

#include <string>

PacketParsingResponse BinaryProtocol::parse_packet(const uint8_t *buffer, uint8_t length) {
    D_WRITE("Packet body: ");
    for (unsigned int i = 0; i < length; ++i) {
        D_PRINTF("%02X ", buffer[i]);
    }
    D_PRINT();

    const auto header_size = sizeof(PacketHeader);
    if (length < header_size) {
        D_PRINTF("Wrong packet size. Expected at least: %u\n", header_size);

        return PacketParsingResponse::fail(Response::code(ResponseCode::PACKET_LENGTH_EXCEEDED));
    }

    auto *packet = (PacketHeader *) buffer;
    if (packet->signature != PACKET_SIGNATURE) {
        D_PRINTF("Wrong packet signature: %X\n", packet->signature);

        return PacketParsingResponse::fail(Response::code(ResponseCode::BAD_REQUEST));
    }

    if (header_size + packet->size != length) {
        D_PRINTF("Wrong message length, expected: %u\n", header_size + packet->size);

        return PacketParsingResponse::fail(Response::code(ResponseCode::BAD_REQUEST));
    }

    const void *data = buffer + header_size;
    return PacketParsingResponse::ok({packet, data});
}

Response BinaryProtocol::update_string_value(char *str, uint8_t max_size, const PacketHeader &header, const void *data) {
    if (header.size > max_size) {
        D_PRINTF("Unable to update value, data too long. Got %u, but limit is %u\n", header.size, max_size);
        return Response::code(ResponseCode::BAD_REQUEST);
    }

    memcpy(str, data, header.size);
    if (header.size < max_size) str[header.size] = '\0';

    D_WRITE("Update parameter ");
    D_WRITE(to_underlying(header.type));
    D_PRINTF(" = %.*s\n", max_size, str);

    return Response::ok();
}

