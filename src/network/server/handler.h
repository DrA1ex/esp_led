#pragma once

#include "lib/network/protocol/packet_handler.h"

#include "app/config.h"
#include "app/metadata.h"
#include "app/application.h"

class PacketHandler : public PacketHandlerBase<Application> {
public:
    PacketHandler(Application &app) : PacketHandlerBase<Application>(app) {}

protected:
    Response handle_packet_data(uint32_t client_id, const Packet<PacketEnumT> &packet) override;

    Response handle_command(PacketHeaderT *header, [[maybe_unused]] const void *data);
};