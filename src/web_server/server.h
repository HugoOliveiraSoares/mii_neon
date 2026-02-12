#pragma once

#include "../effects/Effects.h"
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Ticker.h>
#include <Update.h>
#include <WiFi.h>
#include <map>
#include <vector>

class WebServer {
public:
  WebServer();
  void begin();
  void saveWiFiConfig(const String &ssid, const String &pass);
  bool loadWiFiConfig(String &ssid, String &pass);
  String lastScanJson = "";
  static String wifiStatus;

private:
  AsyncWebServer server;
  Ticker restartTicker;
  void scheduleRestart();
  bool extractTar(File &tarFile);
  void listFiles(const char *dirPath);
};
