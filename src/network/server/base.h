#pragma once

#include "config.h"

#include "network/protocol/binary.h"
#include "network/protocol/type.h"

class Application;

class ServerBase {
    Application &_app;
    BinaryProtocol _protocol;

public:
    explicit ServerBase(Application &app);
    virtual ~ServerBase() = default;

    virtual void handle_incoming_data() = 0;

protected:
    inline Application &app() { return _app; }

    Response handle_packet_data(const uint8_t *buffer, uint16_t length);

private:
    Response _handle_command(PacketHeader *header, const void *data);
    Response _handle_parameter_update(PacketHeader *header, const void *data);
};