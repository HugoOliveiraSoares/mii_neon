#pragma once

#include "../led_service/LedService.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <vector>

class WebServer {
public:
  WebServer();
  void begin();

private:
  AsyncWebServer server;
};
