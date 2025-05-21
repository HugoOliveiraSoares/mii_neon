#pragma once
#include "FastLED.h"
#include <cstdint>

#define NUM_LEDS_STRIP1 140
#define NUM_LEDS_STRIP2 99
#define NUM_TOTAL_LEDS NUM_LEDS_STRIP1 + NUM_LEDS_STRIP2

#define MAX_POWER_MILLIAMPS 500

struct Segment {
  int start;
  int length;
};

class Effects {
public:
  Effects();
  void init();
  void setLedColor(CRGB color, int pos);
  void fill(CRGB color);
  void fill(CRGB color, int length);
  void fillSegment(CRGB color, Segment segment);
  void blink(CRGB color, CRGB color2, int time);
  void blink(CRGB color);
  void blink(CRGB color, CRGB color2);
  void pacifica_loop();
  void cyclon();
  void colorWipe(CRGB color, CRGB color2, int time);
  void colorWipe(CRGB color, int time);
  void colorWipe(CRGB color);
  void snowSparkle(CRGB color, int sparkleDelay, int speedDelay);
  void snowSparkle(CRGB color);
  void rainbowCycle();
  void rainbowCycle(int speedDelay);
  unsigned long getLastUpdate();
  void setLastUpdate(unsigned long newUpdate);

private:
  CRGB leds[NUM_TOTAL_LEDS];
  CRGBPalette16 pacifica_palette_1 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                      0x000210, 0x000212, 0x000114, 0x000117,
                                      0x000019, 0x00001C, 0x000026, 0x000031,
                                      0x00003B, 0x000046, 0x14554B, 0x28AA50};
  CRGBPalette16 pacifica_palette_2 = {0x000507, 0x000409, 0x00030B, 0x00030D,
                                      0x000210, 0x000212, 0x000114, 0x000117,
                                      0x000019, 0x00001C, 0x000026, 0x000031,
                                      0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};
  CRGBPalette16 pacifica_palette_3 = {0x000208, 0x00030E, 0x000514, 0x00061A,
                                      0x000820, 0x000927, 0x000B2D, 0x000C33,
                                      0x000E39, 0x001040, 0x001450, 0x001860,
                                      0x001C70, 0x002080, 0x1040BF, 0x2060FF};
  void pacifica_one_layer(CRGBPalette16 &p, uint16_t cistart,
                          uint16_t wavescale, uint8_t bri, uint16_t ioff);
  void pacifica_add_whitecaps();
  void pacifica_deepen_colors();
  void fadeall();
  uint8_t *wheel(uint8_t wheelPos);
  unsigned long lastUpdate = 0;
  int currentLed = 0;
};
