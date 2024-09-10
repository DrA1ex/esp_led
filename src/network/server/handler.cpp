#include "handler.h"

Response AppPacketHandler::handle_packet_data(uint32_t client_id, const Packet<PacketEnumT> &packet) {
    const auto [header, data] = packet;

    if (header->type == PacketType::GET_CONFIG) {
        return protocol().serialize(app().config());
    } else if (packet.header->type == PacketType::RESTART) {
        app().restart();
        return Response::ok();
    }

    return PacketHandler::handle_packet_data(client_id, packet);
}
