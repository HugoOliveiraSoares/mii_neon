#pragma once

#include "../led_service/LedService.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Ticker.h>
#include <vector>

class WebServer {
public:
  WebServer();
  void begin();
  void scanNetworks();
  void saveWiFiConfig(const String &ssid, const String &pass);
  bool loadWiFiConfig(String &ssid, String &pass);
  String lastScanJson = "";

private:
  AsyncWebServer server;
  Ticker restartTicker;
  void scheduleRestart();
  bool extractTar(File &tarFile);
  void listFiles(const char *dirPath);
};
