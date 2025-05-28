#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <Updater.h>

class UpdateService {
public:
  UpdateService();
  void handleWithUpdate(const String &file, const String &updateType);
  void teste(const String &filePath);

private:
};
