#include "FastLED.h"
#include "effects/Effects.h"
#include "effects/EffectsEnum.h"
#include "led_service/LedService.h"
#include "web_server/server.h"
#include <Arduino.h>
#include <ESP8266mDNS.h>
#include <cstdlib>

#define WIFI_CONFIG_FILE "/wifi.json"
#define AP_SSID ""
#define AP_PASS ""

WebServer webServer;

Effects effects;
LedService ledService;

Segment seg1 = {0, NUM_LEDS_STRIP1};
Segment seg2 = {NUM_LEDS_STRIP1, NUM_LEDS_STRIP2};

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

void setup() {

  Serial.begin(115200);
  if (!LittleFS.begin()) {
    Serial.println("❌ Falha ao montar o sistema de arquivos (LittleFS)!");
    return;
  }

  String ssid, pass;
  bool hasConfig = webServer.loadWiFiConfig(ssid, pass);

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

  // WiFi.begin("ssid", "password");
  // Serial.println("\n🔄 Conectando ao WiFi...");

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.print(".");
  // }
  //
  // Serial.println("\n✅ Conectado ao WiFi!");
  // Serial.print("📡 IP: ");
  // Serial.println(WiFi.localIP());

  if (!MDNS.begin("mii-neon")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  webServer.begin();
  MDNS.addService("http", "tcp", 80);

  effects.init();

  ledService.setBright(128);
  ledService.setColor(CRGB(253, 96, 164));
  effects.fill(ledService.getCurrentColor());

  delay(100);
}

void loop() {
  MDNS.update();

  delay(0);

  switch (ledService.getCurrentEffect()) {
  case Mii:
    effects.fill(ledService.getCurrentColor());
    break;
  case Estatico:
    if (ledService.getCurrentColor() == NULL) {
      ledService.setColor(CRGB(253, 96, 164));
    }
    effects.fill(ledService.getCurrentColor());
    break;
  case Blink:
    effects.blink(ledService.getCurrentColor());
    break;
  case Pacifica:
    effects.pacifica_loop();
    break;
  case Cyclon:
    effects.cyclon();
    break;
  case ColorWipe:
    effects.colorWipe(ledService.getCurrentColor());
    break;
  case ColorWipeInverse:
    effects.colorWipe(ledService.getCurrentColor());
    break;
  case SnowSparkle:
    effects.snowSparkle(ledService.getCurrentColor());
    break;
  case Rainbow:
    effects.rainbowCycle();
    break;
  case EFFECTS_COUNT:
    break;
  }

  FastLED.show();
}
