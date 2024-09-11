#include "api.h"

#include "utils/math.h"


ApiWebServer::ApiWebServer(Application &application, const char *path) : _app(application), _path(path) {}

void ApiWebServer::begin(WebServer &server) {
    _on(server, "/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        auto brightness = map16(_app.config().brightness, DAC_MAX_VALUE, 100);
        response_with_json(request, JsonPropListT{
                {"status",     "ok"},
                {"value",      _app.config().power},
                {"brightness", brightness}
        });
    });

    _on(server, "/power", HTTP_GET, [this](AsyncWebServerRequest *request) {
        bool enabled = request->arg("value") == "1";
        _app.set_power(enabled);

        response_with_json_status(request, "ok");
    });

    _on(server, "/brightness", HTTP_GET, [this](AsyncWebServerRequest *request) {
        auto new_brightness = map16(request->arg("value").toInt(), 100, DAC_MAX_VALUE);

        _app.config().brightness = new_brightness;
        _app.load();

        response_with_json_status(request, "ok");
    });

    _on(server, "/debug", HTTP_GET, [](AsyncWebServerRequest *request) {
        char result[64] = {};

        snprintf(result, sizeof(result), "General:\nHeap: %u\nNow: %lu\n",
                 ESP.getFreeHeap(), millis());

        request->send_P(200, "text/plain", result);
    });

    _on(server, "/restart", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", "OK");

        _app.restart();
    });
}

void ApiWebServer::_on(WebServer &server, const char *uri, WebRequestMethodComposite method, const ArRequestHandlerFunction &onRequest) {
    server.on((_path + uri).c_str(), method, onRequest);
}
