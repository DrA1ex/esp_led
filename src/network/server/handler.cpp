#include "handler.h"

Response PacketHandler::handle_packet_data(uint32_t client_id, const Packet<PacketEnumT> &packet) {
    const auto [header, data] = packet;

    Response response;
    if (header->type == PacketType::GET_CONFIG) {
        response = protocol().serialize(app().config());
    } else if (header->type < PacketType::BRIGHTNESS) {
        response = handle_command(header, data);
    } else {
        response = handle_parameter_update(header, data);
        if (response.is_ok()) {
            if (header->type >= PacketType::NIGHT_MODE_ENABLED && header->type <= PacketType::NIGHT_MODE_BRIGHTNESS) {
                app().night_mode_manager.reset();
            }

            app().update();
        }
    }

    if (response.is_ok() && header->type < PacketType::GET_CONFIG) {
        auto meta_iterator = PacketTypeMetadataMap.find(header->type);
        if (meta_iterator != PacketTypeMetadataMap.end()) {
            app().notify_property_changed(this, meta_iterator->second.property, &client_id);
        } else {
            D_PRINTF("Unsupported notification packet type: %s\n", __debug_enum_str(header->type));
        }
    }

    return response;
}

Response PacketHandler::handle_command(PacketHandlerBase::PacketHeaderT *header, const void *data) {
    switch (header->type) {
        case PacketType::POWER_ON:
            app().set_power(true);
            return Response::ok();

        case PacketType::POWER_OFF:
            app().set_power(false);
            return Response::ok();

        default:
            return Response::code(ResponseCode::BAD_COMMAND);
    }
}
