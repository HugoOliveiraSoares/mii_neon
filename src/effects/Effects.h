#pragma once
#include "../led_strip/LedStrip.h"
#include <cstdint>
#include <memory>
#include <vector>

// Forward declaration for Segment struct
struct Segment {
  int start;
  int length;
};

class Effects {
private:
  std::vector<std::unique_ptr<ILedStrip>> strips;
  unsigned long lastUpdate = 0;

  // Shared effect state for synchronized effects
  struct EffectState {
    bool isOn = true;
    int currentLed = 0;
    bool direction = true;
    int currentHue = 0;
  };
  EffectState state;

  // Cyclon state for synchronized multi-strip operation
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
  // Strip management methods
  template <uint8_t PIN> void addStrip(int numLeds);

  void initAllStrips();
  void showAllStrips();

  // Basic effects (apply to all strips)
  void setLedColor(const CRGB &color, int stripIndex, int pos);
  void fillStrip(const CRGB &color, int stripIndex);
  void fillAllStrips(const CRGB &color);
  void fillSegment(const CRGB &color, int stripIndex, Segment segment);

  // Blink effects
  void blink(const CRGB &color, const CRGB &color2, int time);
  void blink(const CRGB &color);
  void blink(const CRGB &color, const CRGB &color2);

  // ColorWipe effects
  void colorWipe(const CRGB &color, const CRGB &color2, int time);
  void colorWipe(const CRGB &color, int time);
  void colorWipe(const CRGB &color);
  void cyclonUpdatePosition();
  void fadeAllGlobal();

  // Cyclon effect
  void cyclon();

  // RainbowCycle effects
  void rainbowCycle(int speedDelay);
  void rainbowCycle();

  void snowSparkle(CRGB color);
  void snowSparkle(CRGB color, int sparkleDelay);

  // Getters
  unsigned long getLastUpdate() const;
  void setLastUpdate(unsigned long newUpdate);
  int getStripCount() const;
  int getStripLedCount(int stripIndex) const;
};

// Include template implementations
#include "Effects.tpp"
