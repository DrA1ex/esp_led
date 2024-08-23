#pragma once

#include "ESPAsyncWebServer.h"
#include "FS.h"

#include "debug.h"

class WebServer {
    uint16_t _port;
    AsyncWebServer _server;

    FS *_fs = nullptr;

public:
    explicit WebServer(uint16_t port = 80);

    void begin(FS *fs);

    inline void on(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest) {
        _server.on(uri, method, std::move(onRequest));
    }

    inline void add_handler(AsyncWebHandler *handler) { _server.addHandler(handler); }
};