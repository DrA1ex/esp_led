#pragma once

#include "ESPAsyncWebServer.h"
#include "FS.h"

#include "constants.h"
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

class WebAuthHandler : public AsyncWebHandler {
private:
    bool _allow_local = true;

public:
    inline void set_allow_local(bool value) { _allow_local = value; }

    virtual bool canHandle(AsyncWebServerRequest *request) override final;
    virtual void handleRequest(AsyncWebServerRequest *request) override final;
};