#pragma once
#include "../effects/Effects.h"
#include "../effects/EffectsEnum.h"
#include <vector>

class LedService {
public:
  LedService();
  void setColor(CRGB color);
  void setBright(int bright);
  int setMode(String effect);
  std::vector<String> getModes();
  EffectsEnum getCurrentEffect();
  CRGB getCurrentColor();

private:
  EffectsEnum currentEffect;
  CRGB currentColor;
};
