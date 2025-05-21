#include "server.h"

extern LedService ledService;

WebServer::WebServer() : server(80) {}

void WebServer::begin() {
  if (!LittleFS.begin()) {
    Serial.println("❌ Falha ao montar o sistema de arquivos (LittleFS)!");
    return;
  }

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.on(
      "/setColor", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         std::size_t index, std::size_t total) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
          request->send(400, "text/plain", "Invalid JSON");
          return;
        }

        if (!doc.containsKey("rgb")) {
          request->send(400, "application/json",
                        "{\"error\":\"Missing rgb object\"}");
          return;
        }

        int r = doc["rgb"]["r"];
        int g = doc["rgb"]["g"];
        int b = doc["rgb"]["b"];

        ledService.setColor(CRGB(r, g, b));

        request->send(200, "application/json",
                      "{\"response\":\"Color set successfully\"}");
      });

  server.on(
      "/setBright", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         std::size_t index, std::size_t total) {
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
          request->send(400, "application/json",
                        "{\"error\":\"Invalid JSON\"}");
          return;
        }

        if (!doc.containsKey("bright")) {
          request->send(400, "application/json",
                        "{\"error\":\"Missing bright object\"}");
          return;
        }

        int bright = doc["bright"];
        ledService.setBright(bright);

        request->send(200, "application/json",
                      "{\"response\":\"Bright set successfully\"}");
      });

  server.on("/effects", HTTP_GET, [](AsyncWebServerRequest *request) {
    std::vector<String> effectsList = ledService.getModes();

    DynamicJsonDocument jsonDoc(JSON_ARRAY_SIZE(effectsList.size()) +
                                JSON_OBJECT_SIZE(1) + 200);
    JsonArray effectsArray = jsonDoc.createNestedArray("effects");

    for (const String &effectName : effectsList) {
      effectsArray.add(effectName);
    }

    String response;
    serializeJson(jsonDoc, response);
    request->send(200, "application/json", response);
  });

  server.on(
      "/effects", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         std::size_t index, std::size_t total) {
        StaticJsonDocument<200> jsonDoc;

        DeserializationError error = deserializeJson(jsonDoc, data, len);

        if (error) {
          request->send(400, "text/plain", "Invalid JSON");
          return;
        }

        if (!jsonDoc.containsKey("effect")) {
          request->send(400, "application/json",
                        "{\"error\":\"Missing effect object\"}");
          return;
        }

        String effect = jsonDoc["effect"];
        int r = ledService.setMode(effect);
        if (r == -1) { // TODO:  Better error message | throw exception
          request->send(400, "application/json",
                        "{\"error\":\"Invalid Effect name\"}");
        }

        request->send(200, "application/json",
                      "{\"response\":\"Effect set successfully\"}");
      });

  server.begin();

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Resource not found");
  });

  server.begin();
}
