#pragma once

#include <cstdint>

#include "network/enum.h"

enum class ResponseType : uint8_t {
    CODE,
    STRING,
    BINARY,
};

enum class ResponseCode : uint8_t {
    OK,

    BAD_REQUEST,
    BAD_COMMAND,

    TOO_MANY_REQUEST,
    PACKET_LENGTH_EXCEEDED,

    INTERNAL_ERROR
};

struct Response {
    ResponseType type;

    union {
        ResponseCode code;
        const char *str;

        const struct {
            uint16_t size;
            uint8_t *data;
        } buffer;
    } body;

    [[nodiscard]] inline bool is_ok() const { return type != ResponseType::CODE || body.code == ResponseCode::OK; }

    inline static Response ok() {
        return code(ResponseCode::OK);
    };

    inline static Response code(ResponseCode code) {
        return Response{.type = ResponseType::CODE, .body = {.code = code}};
    };

    [[nodiscard]] const char *code_string() const {
        if (type != ResponseType::CODE) return nullptr;

        switch (body.code) {
            case ResponseCode::OK:
                return "OK";

            case ResponseCode::BAD_REQUEST:
                return "BAD REQUEST";

            case ResponseCode::BAD_COMMAND:
                return "BAD COMMAND";

            case ResponseCode::TOO_MANY_REQUEST:
                return "TOO MANY REQUESTS";

            case ResponseCode::PACKET_LENGTH_EXCEEDED:
                return "PACKET LENGTH EXCEEDED";

            case ResponseCode::INTERNAL_ERROR:
            default:
                return "INTERNAL ERROR";
        }
    }
};

struct __attribute__ ((packed))  PacketHeader {
    uint16_t signature;
    PacketType type;
    uint8_t size;
};

struct Packet {
    PacketHeader *header;
    const void *data;
};

struct PacketParsingResponse {
    bool success;

    union {
        Packet packet;
        Response response;
    };

    static PacketParsingResponse ok(Packet &&packet) {
        return PacketParsingResponse{.success = true, .packet = packet};
    }

    static PacketParsingResponse fail(Response &&response) {
        return PacketParsingResponse{.success = false, .response = response};
    }
};