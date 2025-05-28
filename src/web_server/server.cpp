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
        String updateType = "firmware";
        if (request->hasParam("type", true)) {
          updateType = request->getParam("type", true)->value();
        }
        request->send(200, "text/plain", "Atualização concluida.");
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index,
             uint8_t *data, size_t len, bool final) {
        static File uploadFile;

        if (!index) {
          Serial.printf("Upload iniciado: %s\n", filename.c_str());
          uploadFile = LittleFS.open("/firmware.bin", "w");
        }

        if (uploadFile) {
          uploadFile.write(data, len);
        }

        if (final) {
          Serial.printf("Upload concluído: %s, tamanho: %u bytes\n",
                        filename.c_str(), index + len);
          if (uploadFile) {
            uploadFile.close();
          }
          yield();
          updateService.handleWithUpdate("/firmware.bin", "firmware");
        }
      });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Resource not found");
  });

  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"response\":\"AGORA FUNCIONA\"}");
  });

  server.begin();
}
