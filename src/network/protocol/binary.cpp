#include "binary.h"

#include <string>

PacketParsingResponse BinaryProtocol::parse_packet(const uint8_t *buffer, uint8_t length) {
    D_WRITE("Packet body: ");
    D_PRINT_HEX(buffer, length);

    const auto header_size = sizeof(PacketHeader);
    if (length < header_size) {
        D_PRINTF("Wrong packet size. Expected at least: %u\n", header_size);

        return PacketParsingResponse::fail(Response::code(ResponseCode::PACKET_LENGTH_EXCEEDED));
    }

    auto *packet = (PacketHeader *) buffer;
    if (packet->signature != PACKET_SIGNATURE) {
        D_PRINTF("Wrong packet signature: %X\n", packet->signature);

        return PacketParsingResponse::fail(Response::code(ResponseCode::BAD_REQUEST), packet->request_id);
    }

    if (header_size + packet->size != length) {
        D_PRINTF("Wrong message length, expected: %u\n", header_size + packet->size);

        return PacketParsingResponse::fail(Response::code(ResponseCode::BAD_REQUEST), packet->request_id);
    }

    D_PRINTF("---- Packet type: %s\n", __debug_enum_str(packet->type));

    const void *data = buffer + header_size;
    return PacketParsingResponse::ok({packet, data}, packet->request_id);
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

Response BinaryProtocol::update_parameter_value(uint8_t *parameter, uint8_t size, const PacketHeader &header, const void *data) {
    if (header.size != size) {
        D_PRINTF("Unable to update value, bad size. Got %u, expected %u\n", header.size, size);
        return Response::code(ResponseCode::BAD_REQUEST);
    }

    memcpy(parameter, data, size);

    D_WRITE("Update parameter ");
    D_WRITE(__debug_enum_str(header.type));
    D_WRITE(" = ");

    D_PRINT_HEX(parameter, size);

    return Response::ok();
}
