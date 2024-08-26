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

    app().e_property_changed.subscribe(this, [this](auto, auto type, auto arg) {
        auto client_id = arg ? *(uint32_t *) arg : 0;
        switch (type) {
            case PropertyChangedKind::BRIGHTNESS:
                return notify_clients(client_id, PacketType::BRIGHTNESS, app().config.brightness);

            case PropertyChangedKind::POWER:
                return notify_clients(client_id, app().config.power ? PacketType::POWER_ON : PacketType::POWER_OFF);
        }
    });

    D_WRITE("WebSocket server listening on path: ");
    D_PRINT(_path);
}

void WebSocketServer::handle_incoming_data() {
    _ws.cleanupClients();

    while (_request_queue.can_pop()) {
        auto &request = *_request_queue.pop();

        auto response = handle_packet_data(request.client_id, request.data, request.size);
        switch (response.type) {
            case ResponseType::CODE:
                _ws.text(request.client_id, response.code_string());
                break;

            case ResponseType::STRING:
                _ws.text(request.client_id, response.body.str);
                break;

            case ResponseType::BINARY:
                _ws.binary(request.client_id, response.body.buffer.data, (size_t) response.body.buffer.size);
                break;
        }
    }
}

void WebSocketServer::on_event(AsyncWebSocket *server,
                               AsyncWebSocketClient *client,
                               AwsEventType type,
                               void *arg,
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
                _ws.text(client->id(), Response::code(ResponseCode::PACKET_LENGTH_EXCEEDED).code_string());
                return;
            }

            if (len > WS_MAX_PACKET_SIZE) {
                D_PRINTF("Packet dropped. Max packet size %ui\n, but received %ul", WS_MAX_PACKET_SIZE, len);
                _ws.text(client->id(), Response::code(ResponseCode::PACKET_LENGTH_EXCEEDED).code_string());
                return;
            }

            if (!_request_queue.can_acquire()) {
                D_PRINT("Packet dropped. Queue is full");
                _ws.text(client->id(), Response::code(ResponseCode::TOO_MANY_REQUEST).code_string());
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

    (*(PacketHeader *) message) = PacketHeader{PACKET_SIGNATURE, type, size};
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
