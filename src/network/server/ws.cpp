#include "ws.h"

#include "debug.h"


WebSocketServer::WebSocketServer(Application &app, const char *path) : ServerBase(app), _path(path), _ws(path) {}


void WebSocketServer::begin(WebServer &server) {
    auto event_handler =
            [this](auto _1, auto _2, auto _3, auto _4, auto _5, auto _6) {
                on_event(_1, _2, _3, _4, _5, _6);
            };

    _ws.onEvent(event_handler);
    server.add_handler(&_ws);

    D_WRITE("WebSocket server listening on path: ");
    D_PRINT(_path);
}

void WebSocketServer::handle_incoming_data() {
    _ws.cleanupClients();

    while (_request_queue.can_pop()) {
        auto &request = *_request_queue.pop();

        auto response = handle_packet_data(request.data, request.size);
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
