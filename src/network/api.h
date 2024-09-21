#pragma once

#include "lib/network/web.h"

#include "utils/network.h"

class Application;

class ApiWebServer {
    Application &_app;
    String _path;

public:
    ApiWebServer(Application &application, const char *path = "/api");

    void begin(WebServer &server);

protected:
    void _on(WebServer &server, const char *uri, WebRequestMethodComposite method, const ArRequestHandlerFunction &onRequest);
};
