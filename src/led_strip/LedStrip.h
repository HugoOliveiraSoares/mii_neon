#pragma once
#include <FastLED.h>
#include <cstdint>
#include <memory>
#include <vector>

// Abstract interface for LED strip operations
class ILedStrip {
public:
  virtual ~ILedStrip() = default;
  virtual void init() = 0;
  virtual void setLedColor(const CRGB &color, int pos) = 0;
  virtual void setBrightness(int bright) = 0;
  virtual void fill(const CRGB &color) = 0;
  virtual void fill(const CRGB &color, int length) = 0;
  virtual int getNumTotalLeds() const = 0;
  virtual CRGB getCurrentColor() const = 0;
  virtual void show() = 0;
  virtual CRGB *getLeds() = 0;
};

template <uint8_t DATA_PIN> class LedStrip : public ILedStrip {
public:
  explicit LedStrip(int numTotalLeds);

  void init();
  void setLedColor(const CRGB &color, int pos);
  void setBrightness(int bright);
  void fill(const CRGB &color);
  void fill(const CRGB &color, int length);

  int getNumTotalLeds() const;
  CRGB getCurrentColor() const;
  void show();
  CRGB *getLeds();

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
  delay(10);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds.data(), numTotalLeds)
      .setCorrection(TypicalLEDStrip);

  FastLED.setBrightness(200);
  FastLED.clear();
}

template <uint8_t DATA_PIN>
void LedStrip<DATA_PIN>::setLedColor(const CRGB &color, int pos) {
  if (pos < 0 || pos >= numTotalLeds)
    return;

  leds[pos] = color;
  currentColor = color;
}

template <uint8_t DATA_PIN> void LedStrip<DATA_PIN>::setBrightness(int bright) {
  FastLED.setBrightness(bright);
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
}

template <uint8_t DATA_PIN> int LedStrip<DATA_PIN>::getNumTotalLeds() const {
  return numTotalLeds;
}

template <uint8_t DATA_PIN> CRGB LedStrip<DATA_PIN>::getCurrentColor() const {
  return currentColor;
}

template <uint8_t DATA_PIN> void LedStrip<DATA_PIN>::show() { FastLED.show(); }

template <uint8_t DATA_PIN> CRGB *LedStrip<DATA_PIN>::getLeds() {
  return leds.data();
}
