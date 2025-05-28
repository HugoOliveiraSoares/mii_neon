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
      "/color", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
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

  server.on("/color", HTTP_GET, [](AsyncWebServerRequest *request) {
    // TODO: retornar a cor de cada segmento
    CRGB currentColor = ledService.getCurrentColor();

    Serial.print("Get current Color: ");
    Serial.print("R: ");
    Serial.print(currentColor.r);
    Serial.print(", G: ");
    Serial.print(currentColor.g);
    Serial.print(", B: ");
    Serial.println(currentColor.b);

    DynamicJsonDocument doc(128);
    JsonObject rgb = doc.createNestedObject("rgb");
    rgb["r"] = currentColor.r;
    rgb["g"] = currentColor.g;
    rgb["b"] = currentColor.b;

    String jsonString;
    serializeJson(doc, jsonString);

    request->send(200, "application/json", jsonString);
  });

  server.on(
      "/bright", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
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

  server.on("/bright", HTTP_GET, [](AsyncWebServerRequest *request) {
    int currentBright = ledService.getCurrentBright();

    StaticJsonDocument<128> doc;
    doc["bright"] = currentBright;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
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

  server.on(
      "/update", HTTP_POST,
      [this](AsyncWebServerRequest *request) {
        if (Update.hasError()) {
          AsyncWebServerResponse *response = request->beginResponse(
              200, F("text/html"),
              Update.hasError()
                  ? String(F("Update error: ")) + Update.getErrorString()
                  : "Update aborted by server.");
          response->addHeader("Access-Control-Allow-Headers", "*");
          response->addHeader("Access-Control-Allow-Origin", "*");
          response->addHeader("Connection", "close");
          request->send(response);
        } else {
          request->send_P(200, PSTR("text/html"),
                          "Update Success! Rebooting...");
          scheduleRestart();
        }
      },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        static size_t uploadSize = 0;

        if (!index) {
          Serial.printf("Iniciando atualização: %s\n", filename.c_str());
          if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            Update.printError(Serial);
          }
          uploadSize = 0;
        }

        if (Update.write(data, len) != len) {
          Update.printError(Serial);
        }
        uploadSize += len;

        if (final) {
          if (Update.end(true)) {
            Serial.printf("Sucesso: %u bytes\n", uploadSize);
          } else {
            Update.printError(Serial);
          }
        }
      });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Resource not found");
  });

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"response\":\"Agora vai\"}");
  });

  server.begin();
}

void WebServer::scheduleRestart() {
  this->restartTicker.once_ms(1000, [] {
    Serial.println("Reiniciando...");
    ESP.restart();
  });
}
