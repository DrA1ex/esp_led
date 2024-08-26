#include "base.h"

#include "application.h"

ServerBase::ServerBase(Application &app) : _app(app), _protocol() {}


Response ServerBase::handle_packet_data(uint32_t client_id, const uint8_t *buffer, uint16_t length) {
    const auto parseResponse = _protocol.parse_packet(buffer, length);
    if (!parseResponse.success) return parseResponse.response;

    const auto [header, data] = parseResponse.packet;

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

    if (response.is_ok()) {
        switch (header->type) {
            case PacketType::POWER_ON:
            case PacketType::POWER_OFF:
                app().notify_parameter_changed(this, PropertyChangedKind::POWER, &client_id);
                break;

            case PacketType::BRIGHTNESS:
                app().notify_parameter_changed(this, PropertyChangedKind::BRIGHTNESS, &client_id);
                break;

            default:;
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
    switch (header->type) {
        case PacketType::BRIGHTNESS:
            return _protocol.update_parameter_value(&app().config.brightness, *header, data);

        case PacketType::NIGHT_MODE_ENABLED:
            return _protocol.update_parameter_value(&app().config.night_mode.enabled, *header, data);

        case PacketType::NIGHT_MODE_BRIGHTNESS:
            return _protocol.update_parameter_value(&app().config.night_mode.brightness, *header, data);

        case PacketType::NIGHT_MODE_START:
            return _protocol.update_parameter_value(&app().config.night_mode.start_time, *header, data);

        case PacketType::NIGHT_MODE_END:
            return _protocol.update_parameter_value(&app().config.night_mode.end_time, *header, data);

        case PacketType::NIGHT_MODE_INTERVAL:
            return _protocol.update_parameter_value(&app().config.night_mode.switch_interval, *header, data);


        default:
            return Response::code(ResponseCode::BAD_COMMAND);
    }
}
