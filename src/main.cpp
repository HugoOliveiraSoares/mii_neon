#include "FastLED.h"
#include "effects/Effects.h"
#include "effects/EffectsEnum.h"
#include "led_service/LedService.h"
#include "web_server/server.h"
#include <cstdlib>

WebServer webServer;

Effects effects;
LedService ledService;

Segment seg1 = {0, NUM_LEDS_STRIP1};
Segment seg2 = {NUM_LEDS_STRIP1, NUM_LEDS_STRIP2};

void setup() {

  Serial.begin(115200);

  WiFi.begin("Multilaser_2.4G_39E1A8", "smart600w");
  Serial.println("\n🔄 Conectando ao WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\n✅ Conectado ao WiFi!");
  Serial.print("📡 IP: ");
  Serial.println(WiFi.localIP());

  webServer.begin();

  effects.init();
  delay(100);
}

void loop() {

  delay(0);

  switch (ledService.getCurrentEffect()) {
  case Mii:
    effects.fill(ledService.getCurrentColor());
    break;
  case Estatico:
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
