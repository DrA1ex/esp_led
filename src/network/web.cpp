#include "web.h"

class WebLogger : public AsyncWebHandler {
    bool canHandle(AsyncWebServerRequest *request) override {
        D_PRINTF("WebServer: %s -> %s %s\n", request->client()->remoteIP().toString().c_str(),
                 request->methodToString(), request->url().c_str());

        return false;
    }
};

WebServer::WebServer(uint16_t port) : _port(port), _server(port) {
#if DEBUG
    _server.addHandler(new WebLogger());
#endif
}

void WebServer::begin(FS *fs) {
    _fs = fs;

    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(*_fs, "/index.html", "text/html");
    });

    _server.serveStatic("/", *_fs, "/");

    _server.begin();

    D_WRITE("Web server listening on port: ");
    D_PRINT(_port);
}