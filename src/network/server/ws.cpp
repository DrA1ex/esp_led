#include "ws.h"

#include <functional>

#include "application.h"
#include "debug.h"


WebSocketServer::WebSocketServer(Application &app, const char *path) : ServerBase(app), _path(path), _ws(path) {}


void WebSocketServer::begin(WebServer &server) {
    using namespace std::placeholders;
    auto event_handler = std::bind(&WebSocketServer::on_event, this, _1, _2, _3, _4, _5, _6);

    _ws.onEvent(event_handler);
    server.add_handler(&_ws);

    app().e_property_changed.subscribe(this, std::bind(&WebSocketServer::_handle_notification, this, _1, _2, _3));

    D_WRITE("WebSocket server listening on path: ");
    D_PRINT(_path);
}

void WebSocketServer::handle_incoming_data() {
    _ws.cleanupClients();

    while (_request_queue.can_pop()) {
        auto &request = *_request_queue.pop();

        auto parsingResponse = parse_packet(request.data, request.size);

        Response response;
        if (parsingResponse.success) {
            response = handle_packet_data(request.client_id, parsingResponse.packet);
        } else {
            response = parsingResponse.response;
        }

        return _send_response(request.client_id, parsingResponse.request_id, response);
    }
}

void WebSocketServer::on_event(AsyncWebSocket *,
                               AsyncWebSocketClient *client,
                               AwsEventType type,
                               void *,
                               uint8_t *data,
                               size_t len) {

    switch (type) {
        case WS_EVT_CONNECT:
            D_PRINTF("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;

        case WS_EVT_DISCONNECT:
            D_PRINTF("WebSocket client #%u disconnected\n", client->id());
            break;

        case WS_EVT_DATA: {
            D_PRINTF("Received WebSocket packet, size: %u\n", len);

            if (len == 0) {
                _send_response(client->id(), 0, Response::code(ResponseCode::PACKET_LENGTH_EXCEEDED));
                return;
            }

            if (len > WS_MAX_PACKET_SIZE) {
                D_PRINTF("Packet dropped. Max packet size %ui\n, but received %ul", WS_MAX_PACKET_SIZE, len);
                _send_response(client->id(), 0, Response::code(ResponseCode::PACKET_LENGTH_EXCEEDED));
                return;
            }

            if (!_request_queue.can_acquire()) {
                D_PRINT("Packet dropped. Queue is full");
                _send_response(client->id(), 0, Response::code(ResponseCode::TOO_MANY_REQUEST));
                return;
            }

            auto &request = *_request_queue.acquire();

            request.client_id = client->id();
            request.size = len;
            memcpy(request.data, data, len);

            break;
        }

        default:
            break;
    }
}

void WebSocketServer::notify_clients(uint32_t sender_id, PacketType type) {
    notify_clients(sender_id, type, nullptr, 0);
}

void WebSocketServer::notify_clients(uint32_t sender_id, PacketType type, const void *data, uint8_t size) {
    uint8_t message[sizeof(PacketHeader) + size];

    (*(PacketHeader *) message) = PacketHeader{PACKET_SIGNATURE, 0, type, size};
    mempcpy(message + sizeof(PacketHeader), data, size);

    D_PRINTF("Send message total size: %u (data size: %u)\n", sizeof(message), size);

    for (auto &client: _ws.getClients()) {
        if (sender_id == client->id()) continue;

        _ws.binary(client->id(), message, sizeof(message));
    }
}

template<typename T>
void WebSocketServer::notify_clients(uint32_t sender_id, PacketType type, const T &value) {
    D_PRINTF("Send value message size: %u\n", sizeof(value));

    notify_clients(sender_id, type, &value, sizeof(value));
}

void WebSocketServer::_send_response(uint32_t client_id, uint16_t request_id, const Response &response) {
    auto header = PacketHeader{
            .signature = PACKET_SIGNATURE,
            .request_id = request_id
    };

    const void *data;

    switch (response.type) {
        case ResponseType::CODE:
            header.type = PacketType::RESPONSE_STRING;
            data = (void *) response.code_string();
            header.size = strlen((const char *) data);
            break;

        case ResponseType::STRING:
            header.type = PacketType::RESPONSE_STRING;
            header.size = strlen(response.body.str);
            data = (void *) response.body.str;
            break;

        case ResponseType::BINARY:
            if (response.body.buffer.size > 255) {
                D_PRINTF("Response size too long: %u", response.body.buffer.size);
                return _send_response(client_id, request_id, Response::code(ResponseCode::INTERNAL_ERROR));
            }

            header.type = PacketType::RESPONSE_BINARY;
            header.size = response.body.buffer.size;
            data = (void *) response.body.buffer.data;
            break;

        default:
            D_PRINTF("Unknown response type %u", (uint8_t) response.type);
            return _send_response(client_id, request_id, Response::code(ResponseCode::INTERNAL_ERROR));
    }


    uint8_t response_data[sizeof(header) + header.size];
    memcpy(response_data, &header, sizeof(header));
    memcpy(response_data + sizeof(header), data, header.size);

    _ws.binary(client_id, response_data, sizeof(response_data));
}

void WebSocketServer::_handle_notification(void *, NotificationProperty type, void *arg) {
    auto prop_iterator = PropertyMetadataMap.find(type);
    if (prop_iterator == PropertyMetadataMap.end()) {
        D_PRINTF("Unsupported notification type %u\n", (uint8_t) type);
        return;
    }

    const std::vector<PropertyMetadata> &prop = prop_iterator->second;
    auto client_id = arg ? *(uint32_t *) arg : 0;

    const auto &meta = prop[0];

    D_PRINTF("Preparing notification data for %u, size: %u, offset: %u\n", type, meta.value_size, meta.value_offset);

    // Copy data to avoid unaligned memory access
    uint8_t data[meta.value_size];
    memcpy(data, (uint8_t *) &app().config + meta.value_offset, meta.value_size);

    if (prop.size() == 1) {
        notify_clients(client_id, meta.packet_type, data, meta.value_size);
    } else {
        if (meta.value_size != 1) {
            D_PRINTF("Unsupported notification for trigger type %u. Expected size 1 byte, but got %u\n",
                     (uint8_t) type, meta.value_size);
            return;
        }

        bool trigger = *(bool *) data;
        notify_clients(client_id, trigger ? meta.packet_type : prop[1].packet_type);
    }
}
