#include "base.h"

#include "application.h"

ServerBase::ServerBase(Application &app) : _app(app), _protocol() {}

PacketParsingResponse ServerBase::parse_packet(const uint8_t *buffer, uint16_t length) {
    return _protocol.parse_packet(buffer, length);
}

Response ServerBase::handle_packet_data(uint32_t client_id, const uint8_t *buffer, uint16_t length) {
    const auto parseResponse = parse_packet(buffer, length);
    if (!parseResponse.success) return parseResponse.response;

    return handle_packet_data(client_id, parseResponse.packet);
}

Response ServerBase::handle_packet_data(uint32_t client_id, const Packet &packet) {
    const auto [header, data] = packet;

    Response response;
    if (header->type == PacketType::GET_CONFIG) {
        response = _protocol.serialize(app().config);
    } else if (header->type < PacketType::BRIGHTNESS) {
        response = _handle_command(header, data);
    } else {
        response = _handle_parameter_update(header, data);
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
            app().notify_parameter_changed(this, meta_iterator->second.property, &client_id);
        } else {
            D_PRINTF("Unsupported notification packet type: %s\n", __debug_enum_str(header->type));
        }
    }

    return response;
}

Response ServerBase::_handle_command(PacketHeader *header, const void *) {
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

Response ServerBase::_handle_parameter_update(PacketHeader *header, const void *data) {
    auto meta_iter = PacketTypeMetadataMap.find(header->type);

    if (meta_iter == PacketTypeMetadataMap.end()) {
        D_PRINTF("Received unsupported parameter: %u (%s)\n", _protocol.to_underlying(header->type), __debug_enum_str(header->type));
        return Response::code(ResponseCode::BAD_COMMAND);
    }
    auto &meta = meta_iter->second;
    return _protocol.update_parameter_value(((uint8_t * )(&app().config)) + meta.value_offset, meta.value_size, *header, data);
}
