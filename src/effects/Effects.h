#pragma once
#include "../led_strip/LedStrip.h"
#include "EffectsEnum.h"
#include <cstdint>
#include <memory>
#include <vector>

class Effects {
private:
  std::vector<std::unique_ptr<ILedStrip>> strips;
  unsigned long lastUpdate = 0;
  EffectsEnum currentEffect;
  CRGB currentColor;
  int currentBright;

  struct EffectState {
    bool isOn = true;
    int currentLed = 0;
    bool direction = true;
    int currentHue = 0;
  };
  EffectState state;

  struct CyclonState {
    int currentLed = 0;
    bool direction = true;
    int hue = 0;
    static const int UPDATE_INTERVAL = 10; // 10ms timing
  };
  CyclonState cyclonState;

  struct RainbowCycleState {
    uint16_t phaseOffset = 0;
    unsigned long lastUpdate = 0;
  };
  RainbowCycleState rainbowState;

public:
  template <uint8_t PIN> void addStrip(int numLeds);

  void initAllStrips();
  void showAllStrips();

  void setColor(CRGB color);
  CRGB getCurrentColor();

  void setBrightness(int bright);
  int getBrightness();

  unsigned long getLastUpdate() const;
  void setLastUpdate(unsigned long newUpdate);

  int getStripCount() const;
  int getStripLedCount(int stripIndex) const;

  EffectsEnum getCurrentEffect();
  int setCurrentEffect(String effectStr);

  void setLedColor(const CRGB &color, int stripIndex, int pos);
  void fillStrip(const CRGB &color, int stripIndex);
  void fillAllStrips(const CRGB &color);

  void blink(const CRGB &color, const CRGB &color2, int time);
  void blink(const CRGB &color);
  void blink(const CRGB &color, const CRGB &color2);

  void colorWipe(const CRGB &color, const CRGB &color2, int time,
                 bool isReverse);
  void colorWipe(const CRGB &color, int time);
  void colorWipe(const CRGB &color);
  void colorWipeReverse(const CRGB &color);

  void cyclonUpdatePosition();

  void fadeAllGlobal();

  void cyclon();

  void rainbowCycle(int speedDelay);
  void rainbowCycle();

  void snowSparkle(CRGB color);
  void snowSparkle(CRGB color, int sparkleDelay);
};

#include "Effects.tpp"
