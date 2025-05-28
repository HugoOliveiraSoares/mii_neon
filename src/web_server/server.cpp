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

  server.on(
      "/fsupdate", HTTP_POST,
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
          request->send_P(200, "text/html", "FS Update Success! Rebooting...");
          scheduleRestart();
        }

        scheduleRestart();
      },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        static size_t uploadSize = 0;

        if (!index) {
          Serial.printf("Iniciando atualização FS: %s\n", filename.c_str());
          if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000,
                            U_FS)) {
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
            Serial.printf("Sucesso FS: %u bytes\n", uploadSize);
          } else {
            Update.printError(Serial);
          }
        }
      });

  server.on(
      "/upload_tar", HTTP_POST,
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

          // Agora processa e extrai o TAR
          File f = LittleFS.open("/temp_upload.tar", "r");
          if (f) {
            if (extractTar(f)) {
              Serial.println("Extração concluída com sucesso.");
            } else {
              Serial.println("Erro na extração.");
            }
            f.close();
            LittleFS.remove(
                "/temp_upload.tar"); // Opcional: remove TAR após extração
          } else {
            Serial.println("Falha ao abrir o arquivo TAR.");
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
