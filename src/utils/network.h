#pragma once

#include <variant>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

typedef std::variant<bool, int, long, long long, float, double, const char *> JsonPropVariantT;
typedef std::initializer_list<std::pair<const char *, JsonPropVariantT>> JsonPropListT;

inline void response_with_json(AsyncWebServerRequest *request, JsonDocument &doc) {
    auto *response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);
}


inline void response_with_json(AsyncWebServerRequest *request, JsonPropListT props) {
    JsonDocument doc;
    for (auto prop: props) {
        std::visit([&](auto &&arg) { doc[prop.first] = arg; }, prop.second);
    }

    response_with_json(request, doc);
}

inline void response_with_json_status(AsyncWebServerRequest *request, const char *status) {
    JsonDocument doc;
    doc["status"] = status;

    response_with_json(request, doc);
}
