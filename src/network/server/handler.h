#pragma once

#include "lib/network/protocol/packet_handler.h"

#include "app/application.h"

class AppPacketHandler : public PacketHandler<Application> {
public:
    AppPacketHandler(Application &app) : PacketHandler(app) {}

protected:
    Response handle_packet_data(uint32_t client_id, const Packet<PacketEnumT> &packet) override;
};
