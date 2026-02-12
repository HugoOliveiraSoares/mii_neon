#include "FastLED.h"
#include "effects/Effects.h"
#include "effects/EffectsEnum.h"
#include "web_server/server.h"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>

#define WIFI_CONFIG_FILE "/wifi.json"
#define AP_SSID "Mii-Neon"
#define AP_PASS ""

WebServer webServer;

Effects effects;

unsigned long lastScanTime = 0;
const unsigned long scanInterval = 10000; // 10 segundos

void startAP() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.println("AP iniciado!");
  Serial.print("SSID: ");
  Serial.println(AP_SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());
}

void startWifi() {

  String ssid, pass;
  bool hasConfig = false;
#if DEV_ENV
  ssid = "";
  pass = "";
  hasConfig = true;
#else
  hasConfig = webServer.loadWiFiConfig(ssid, pass);
#endif

  if (hasConfig) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.print("Conectando em ");
    Serial.println(ssid);
    Serial.println(pass);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi conectado!");
      WebServer::wifiStatus = "success";
      Serial.println(WiFi.localIP());
    } else {
      WebServer::wifiStatus = "fail";
      Serial.println("\nErro: Não foi possível conectar na rede salva.");
      startAP();
    }
  } else {
    startAP();
  }
}

void setup() {

  Serial.begin(115200);
  delay(500);
  Serial.printf("Starting in mode: %s\n", ENV_MODE);

  if (!LittleFS.begin()) {
    Serial.println("❌ Falha ao montar o sistema de arquivos (LittleFS)!");
    return;
  }

  startWifi();

  if (!MDNS.begin("mii-neon")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  webServer.begin();
  MDNS.addService("http", "tcp", 80);

  effects.addStrip<13>(2);
  effects.addStrip<14>(2);
  effects.addStrip<16>(9);
  effects.addStrip<17>(4);
  effects.addStrip<18>(27);
  effects.addStrip<19>(5);
  effects.addStrip<21>(5);
  effects.addStrip<22>(8);
  effects.addStrip<23>(5);
  effects.addStrip<25>(5);
  effects.addStrip<32>(7);
  effects.addStrip<27>(5);
  effects.initAllStrips();

  effects.showAllStrips();

  effects.loadConfig();

  delay(100);
}

void loop() {

  switch (effects.getCurrentEffect()) {
  case Mii:
    effects.miiEffect();
    break;
  case Estatico:
    effects.estaticEffect();
    break;
  case Blink:
    if (effects.getCurrentColor() == NULL) {
      effects.setColor(CRGB(253, 96, 164));
    }
    effects.blink(effects.getCurrentColor());
    break;
  case Cyclon:
    effects.cyclon();
    break;
  case ColorWipe:
    effects.colorWipe(CRGB::Green);
    break;
  case ColorWipeReverse:
    effects.colorWipeReverse(CRGB::Green);
    break;
  case SnowSparkle:
    effects.snowSparkle(CRGB::Cyan, 15);
    break;
  case Rainbow:
    effects.rainbowCycle();
    break;
  case EFFECTS_COUNT:
    break;
  }

  effects.showAllStrips();
}
