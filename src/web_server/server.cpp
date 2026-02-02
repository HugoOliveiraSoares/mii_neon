#include "server.h"
#include "../effects/EffectErrorCodes.h"

extern Effects effects;

String WebServer::wifiStatus = "idle";

String buildErrorResponse(const String &type, const String &message,
                          int statusCode, const String &details = "",
                          const String &invalidEffect = "") {
  StaticJsonDocument<768> doc;

  JsonObject error = doc.createNestedObject("error");
  error["type"] = type;
  error["message"] = message;
  error["status_code"] = statusCode;
  error["path"] = "/effect";

  if (!invalidEffect.isEmpty()) {
    JsonObject detailsObj = error.createNestedObject("details");
    detailsObj["invalid_effect"] = invalidEffect;

    // Generate available effects array from EffectsEnum
    JsonArray effectsArray = detailsObj.createNestedArray("available_effects");
    for (int i = 0; i < EFFECTS_COUNT; i++) {
      EffectsEnum effect = static_cast<EffectsEnum>(i);
      effectsArray.add(toString(effect));
    }
  }

  if (!details.isEmpty() && invalidEffect.isEmpty()) {
    error["details"] = details;
  }

  String response;
  serializeJson(doc, response);
  return response;
}

// Helper function to log errors
void logError(const String &type, const String &message,
              const String &context = "") {
#ifdef DEV_ENV
  if (!context.isEmpty()) {
    Serial.println("[CONTEXT] " + context);
  }
  Serial.println("[ENDPOINT] /effect (POST)");
  Serial.println("----------------------------------------");
#else
  Serial.println("[ERROR] " + type + ": " + message);
#endif
}

WebServer::WebServer() : server(80) {}
void WebServer::begin() {

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

        effects.setColor(CRGB(r, g, b));

        request->send(200, "application/json",
                      "{\"response\":\"Color set successfully\"}");
      });

  server.on("/color", HTTP_GET, [](AsyncWebServerRequest *request) {
    CRGB currentColor = effects.getCurrentColor();

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
        effects.setBrightness(bright);

        request->send(200, "application/json",
                      "{\"response\":\"Bright set successfully\"}");
      });

  server.on("/bright", HTTP_GET, [](AsyncWebServerRequest *request) {
    int currentBright = effects.getBrightness();

    StaticJsonDocument<128> doc;
    doc["bright"] = currentBright;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/effect", HTTP_GET, [](AsyncWebServerRequest *request) {
    EffectsEnum currentEffect = effects.getCurrentEffect();

    StaticJsonDocument<128> doc;
    doc["current_effect"] = toString(currentEffect);

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on(
      "/effect", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
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

        // Use error code checking instead of exceptions to avoid ESP8266
        // crashes
        int result = effects.setCurrentEffect(effect);

        if (result == EFFECT_SUCCESS) {
          // Success response with timestamp
          StaticJsonDocument<256> successDoc;
          successDoc["response"] = "Effect set successfully";
          successDoc["effect"] = effect;

          String successResponse;
          serializeJson(successDoc, successResponse);
          request->send(200, "application/json", successResponse);

        } else if (result == EFFECT_INVALID_NAME) {
          // Handle invalid effect with RESTful response
          logError("InvalidEffect", ERROR_INVALID_NAME_STR,
                   "Effect: " + effect);

          String errorResponse =
              buildErrorResponse("InvalidEffectException",
                                 ERROR_INVALID_NAME_STR, 400, "", effect);

          request->send(400, "application/json", errorResponse);

        } else {
          // Handle any other unexpected errors
          logError("UnknownError", "Unknown error occurred",
                   "Effect: " + effect);

          String errorResponse = buildErrorResponse(
              "UnknownError", "An unknown error occurred", 500);

          request->send(500, "application/json", errorResponse);
        }
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

  server.on(
      "/fsupdate", HTTP_POST,
      [this](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Arquivo TAR recebido e processado.");
        listFiles("/");
      },
      [this](AsyncWebServerRequest *request, String filename, size_t index,
             uint8_t *data, size_t len, bool final) {
        static File tarFile;

        if (index == 0) {
          Serial.printf("Iniciando upload de %s\n", filename.c_str());
          tarFile = LittleFS.open("/temp_upload.tar", "w");
        }

        if (tarFile) {
          tarFile.write(data, len);
        }

        if (final) {
          Serial.printf("Upload finalizado: %s, tamanho: %u bytes\n",
                        filename.c_str(), index + len);
          tarFile.close();

          File f = LittleFS.open("/temp_upload.tar", "r");
          if (f) {
            if (extractTar(f)) {
              Serial.println("Extração concluída com sucesso.");
            } else {
              Serial.println("Erro na extração.");
            }
            f.close();
            LittleFS.remove("/temp_upload.tar");
          } else {
            Serial.println("Falha ao abrir o arquivo TAR.");
          }
        }
      });

  server.on("/savewifi", HTTP_POST, [this](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String ssid = request->getParam("ssid", true)->value();
      String pass = request->getParam("pass", true)->value();
      saveWiFiConfig(ssid, pass);
      request->send_P(200, PSTR("application/json"),
                      "{\"status\":\"Credentials saved! Rebooting...\"}");
      scheduleRestart();
    } else {
      request->send(400, PSTR("application/json"),
                    "{\"status\":\"Parâmetros inválidos\"}");
    }
  });

  server.on("/scan", HTTP_GET, [this](AsyncWebServerRequest *request) {
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_RUNNING) {
      request->send(200, "application/json", "{\"status\":\"scanning\"}");
      return;
    }
    if (n >= 0) {
      DynamicJsonDocument doc(1024);
      JsonArray arr = doc.createNestedArray("networks");
      for (int i = 0; i < n; ++i) {
        arr.add(WiFi.SSID(i));
      }
      WiFi.scanDelete();
      String json;
      serializeJson(doc, json);
      request->send(200, "application/json", json);
      return;
    }

    WiFi.scanNetworks(true);
    request->send(200, "application/json", "{\"status\":\"started\"}");
  });

  server.on("/wifistatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(128);
    doc["status"] = wifiStatus;
    if (wifiStatus == "success")
      doc["ip"] = WiFi.localIP().toString();
    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "application/json", "{\"msg\":\"Resource not found\"}");
  });

  server.begin();
}

void WebServer::scheduleRestart() {
  this->restartTicker.once_ms(1000, [] {
    Serial.println("Reiniciando...");
    ESP.restart();
  });
}

bool WebServer::extractTar(File &tarFile) {
  const size_t BLOCK_SIZE = 512;
  uint8_t header[BLOCK_SIZE];

  Serial.printf("Tamanho do tar: %u bytes\n", tarFile.size());

  while (tarFile.position() < tarFile.size()) {
    if (tarFile.read(header, BLOCK_SIZE) != BLOCK_SIZE) {
      Serial.println("Erro ao ler cabeçalho TAR.");
      return false;
    }

    if (header[0] == '\0') {
      Serial.println("Fim do arquivo TAR detectado.");
      break;
    }

    // Nome do arquivo
    char name[101] = {0};
    memcpy(name, header, 100);

    // Tamanho do arquivo (em octal)
    char sizeStr[13] = {0};
    memcpy(sizeStr, header + 124, 12);
    unsigned long fileSize = strtoul(sizeStr, NULL, 8);

    Serial.printf("Extraindo: %s (%lu bytes)\n", name, fileSize);

    // Ignora diretórios
    if (name[strlen(name) - 1] == '/') {
      continue;
    }

    // Garante que diretórios existam
    String filePath = "/" + String(name);
    int lastSlash = filePath.lastIndexOf('/');
    if (lastSlash != -1) {
      String dirPath = filePath.substring(0, lastSlash);
      if (!LittleFS.exists(dirPath)) {
        LittleFS.mkdir(dirPath);
        Serial.printf("Criado diretório: %s\n", dirPath.c_str());
      }
    }

    File outFile = LittleFS.open(filePath, "w");
    if (!outFile) {
      Serial.printf("Erro ao criar arquivo: %s\n", filePath.c_str());
      return false;
    }

    unsigned long remaining = fileSize;
    uint8_t buf[BLOCK_SIZE];

    while (remaining > 0) {
      size_t toRead = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
      if (tarFile.read(buf, toRead) != toRead) {
        Serial.println("Erro de leitura durante extração.");
        outFile.close();
        return false;
      }
      outFile.write(buf, toRead);
      remaining -= toRead;
    }
    outFile.close();

    // Pula o padding
    size_t padding = (BLOCK_SIZE - (fileSize % BLOCK_SIZE)) % BLOCK_SIZE;
    if (padding) {
      tarFile.seek(padding, SeekCur);
    }
  }
  Serial.println("Extração concluída com sucesso.");
  return true;
}

void WebServer::listFiles(const char *dirPath) {
  Dir dir = LittleFS.openDir(dirPath);
  while (dir.next()) {
    Serial.printf("Arquivo: %s (%d bytes)\n", dir.fileName().c_str(),
                  dir.fileSize());
  }
}

void WebServer::saveWiFiConfig(const String &ssid, const String &pass) {
  Serial.println("Salvando credenciais do wifi");
  DynamicJsonDocument doc(256);
  doc["ssid"] = ssid;
  doc["pass"] = pass;
  File f = LittleFS.open("/wifi.json", "w");
  serializeJson(doc, f);
  f.close();
  Serial.println("Credenciais salvas!");
}

bool WebServer::loadWiFiConfig(String &ssid, String &pass) {
  if (!LittleFS.exists("/wifi.json"))
    return false;
  File f = LittleFS.open("/wifi.json", "r");
  DynamicJsonDocument doc(256);
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err)
    return false;
  ssid = doc["ssid"].as<String>();
  pass = doc["pass"].as<String>();
  return true;
}
