#include "LedService.h"

extern Effects effects;

LedService::LedService() {}

void LedService::setColor(CRGB color) {
  this->currentColor = color;
  Serial.println("Cor definido");
}

void LedService::setBright(int bright) {

  FastLED.setBrightness(bright);
  FastLED.show();
  Serial.println("Brilho definido");
}

int LedService::setMode(String effectStr) {

  EffectsEnum effect = fromString(effectStr);

  if (effect == static_cast<EffectsEnum>(-1)) {
    return -1; // TODO:  Better error message | throw exception
  }

  effects.setLastUpdate(0);

  this->currentEffect = effect;
  Serial.print("Efeito definido: ");
  Serial.println(toString(effect));

  return 0;
}

std::vector<String> LedService::getModes() {
  std::vector<String> effectsNames;
  for (int i = 0; i < EFFECTS_COUNT; i++) {
    EffectsEnum effects = static_cast<EffectsEnum>(i);
    effectsNames.push_back(toString(effects));
  }
  return effectsNames;
}

EffectsEnum LedService::getCurrentEffect() { return this->currentEffect; }
CRGB LedService::getCurrentColor() { return this->currentColor; }
