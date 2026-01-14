#pragma once
#include <FastLED.h>
#include <cstdint>
#include <vector>

template <uint8_t DATA_PIN> class LedStrip {
public:
  explicit LedStrip(int numTotalLeds);

  void init();
  void setLedColor(const CRGB &color, int pos);
  void fill(const CRGB &color);
  void fill(const CRGB &color, int length);

  int getNumTotalLeds() const;
  CRGB getCurrentColor() const;

private:
  std::vector<CRGB> leds;
  int numTotalLeds;
  CRGB currentColor;
};

template <uint8_t DATA_PIN>
LedStrip<DATA_PIN>::LedStrip(int numTotalLeds)
    : numTotalLeds(numTotalLeds), currentColor(CRGB(0, 0, 0)) {
  leds.resize(numTotalLeds);
}

template <uint8_t DATA_PIN> void LedStrip<DATA_PIN>::init() {
  delay(500);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds.data(), numTotalLeds)
      .setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(20);
  FastLED.clear();
}

template <uint8_t DATA_PIN>
void LedStrip<DATA_PIN>::setLedColor(const CRGB &color, int pos) {
  if (pos < 0 || pos >= numTotalLeds)
    return;

  leds[pos] = color;
  currentColor = color;
  FastLED.show();
}

template <uint8_t DATA_PIN> void LedStrip<DATA_PIN>::fill(const CRGB &color) {
  fill(color, numTotalLeds);
}

template <uint8_t DATA_PIN>
void LedStrip<DATA_PIN>::fill(const CRGB &color, int length) {
  length = min(length, numTotalLeds);

  for (int i = 0; i < length; i++) {
    leds[i] = color;
  }

  currentColor = color;
  FastLED.show();
}

template <uint8_t DATA_PIN> int LedStrip<DATA_PIN>::getNumTotalLeds() const {
  return numTotalLeds;
}

template <uint8_t DATA_PIN> CRGB LedStrip<DATA_PIN>::getCurrentColor() const {
  return currentColor;
}
