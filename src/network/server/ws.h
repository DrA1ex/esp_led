#pragma once

#include <queue>
#include <AsyncWebSocket.h>

#include "base.h"
#include "metadata.h"
#include "misc/circular_buffer.h"
#include "network/web.h"

struct WebSocketRequest {
    uint32_t client_id = 0;
    size_t size = 0;
    uint8_t data[WS_MAX_PACKET_SIZE] = {};
};

class WebSocketServer : ServerBase {
    CircularBuffer<WebSocketRequest, WS_MAX_QUEUED_MESSAGES> _request_queue;

    const char *_path;
    AsyncWebSocket _ws;

public:
    explicit WebSocketServer(Application &app, const char *path = "/ws");

    void begin(WebServer &server);

    void handle_incoming_data() override;

protected:
    void on_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

    void notify_clients(uint32_t sender_id, PacketType type);
    void notify_clients(uint32_t sender_id, PacketType type, const void *data, uint8_t size);

    template<typename T>
    void notify_clients(uint32_t sender_id, PacketType type, const T &value);

private:
    void _send_response(uint32_t client_id, uint16_t request_id, const Response &response);
    void _handle_notification(void *sender, NotificationProperty type, void *arg);
};
