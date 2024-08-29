#pragma once

#include "constants.h"
#include "app/application.h"

#include "network/web.h"

#include "utils/network.h"

class ApiWebServer {
    Application &_app;
    const char *_path;

public:
    ApiWebServer(Application &application, const char *path = "/api");

    void begin(WebServer &server);
};