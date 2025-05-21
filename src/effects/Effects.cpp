#include "Effects.h"

Effects::Effects() {}

void Effects::init() {

  delay(1500);
  FastLED.addLeds<WS2812B, 2, GRB>(leds, 0, NUM_LEDS_STRIP1)
      .setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, 0, GRB>(leds, NUM_LEDS_STRIP1, NUM_LEDS_STRIP2)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  FastLED.clear();
}

unsigned long Effects::getLastUpdate() { return this->lastUpdate; }

void Effects::setLastUpdate(unsigned long newUpdate) {
  this->lastUpdate = newUpdate;
}

void Effects::setLedColor(CRGB color, int pos) {
  this->leds[pos] = color;
  FastLED.show();
}

void Effects::fill(CRGB color) { this->fill(color, NUM_TOTAL_LEDS); }

void Effects::fill(CRGB color, int length) {
  for (int i = 0; i < length; i++) {
    this->leds[i] = color;
  }
  FastLED.show();
}

void Effects::fillSegment(CRGB color, Segment segment) {
  int endPos = segment.start + segment.length;

  // Garante que o segmento não ultrapasse os limites do array
  if (segment.start < 0 || segment.start >= NUM_TOTAL_LEDS)
    return;
  if (endPos > NUM_TOTAL_LEDS)
    endPos = NUM_TOTAL_LEDS;

  for (int i = segment.start; i < endPos; i++) {
    this->leds[i] = color;
  }

  FastLED.show();
}

void Effects::blink(CRGB color, CRGB color2, int time) {
  static bool isOn;
  if (millis() - this->lastUpdate > time) {
    if (isOn) {
      this->fill(color2, NUM_TOTAL_LEDS);
      isOn = false;
    } else {
      this->fill(color, NUM_TOTAL_LEDS);
      isOn = true;
    }
    this->lastUpdate = millis();
  }
}

void Effects::blink(CRGB color) { this->blink(color, CRGB::Black, 1000); }
void Effects::blink(CRGB color, CRGB color2) {
  this->blink(color, color2, 1000);
}

void Effects::pacifica_loop() {
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
  sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
  sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
  sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

  // Clear out the LED array to a dim background blue-green
  fill_solid(leds, NUM_TOTAL_LEDS, CRGB(2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary
  // over time
  this->pacifica_one_layer(this->pacifica_palette_1, sCIStart1,
                           beatsin16(3, 11 * 256, 14 * 256),
                           beatsin8(10, 70, 130), 0 - beat16(301));
  this->pacifica_one_layer(this->pacifica_palette_2, sCIStart2,
                           beatsin16(4, 6 * 256, 9 * 256), beatsin8(17, 40, 80),
                           beat16(401));
  this->pacifica_one_layer(this->pacifica_palette_3, sCIStart3, 6 * 256,
                           beatsin8(9, 10, 38), 0 - beat16(503));
  this->pacifica_one_layer(this->pacifica_palette_3, sCIStart4, 5 * 256,
                           beatsin8(8, 10, 28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  this->pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  this->pacifica_deepen_colors();
  FastLED.show();
}

void Effects::pacifica_one_layer(CRGBPalette16 &p, uint16_t cistart,
                                 uint16_t wavescale, uint8_t bri,
                                 uint16_t ioff) {
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for (uint16_t i = 0; i < NUM_TOTAL_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16(waveangle) + 32768;
    uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16(ci) + 32768;
    uint8_t sindex8 = scale16(sindex16, 240);
    CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
    this->leds[i] += c;
  }
}

void Effects::pacifica_add_whitecaps() {
  uint8_t basethreshold = beatsin8(9, 55, 65);
  uint8_t wave = beat8(7);

  for (uint16_t i = 0; i < NUM_TOTAL_LEDS; i++) {
    uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = this->leds[i].getAverageLight();
    if (l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8(overage, overage);
      this->leds[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void Effects::pacifica_deepen_colors() {
  for (uint16_t i = 0; i < NUM_TOTAL_LEDS; i++) {
    this->leds[i].blue = scale8(leds[i].blue, 145);
    this->leds[i].green = scale8(leds[i].green, 200);
    this->leds[i] |= CRGB(2, 5, 7);
  }
}

void Effects::cyclon() {
  unsigned long currentMillis = millis();
  static int currentLed = 0;
  static bool direction = true;
  static int hue = 0;

  // Verifica se o tempo desde a última atualização é maior ou igual ao
  // intervalo desejado (10ms)
  if (currentMillis - this->lastUpdate >= 10) {
    this->lastUpdate = currentMillis; // Atualiza o tempo da última atualização

    if (direction) {
      // Movimento da esquerda para a direita
      leds[currentLed] = CHSV(hue++, 255, 255); // Define a cor do LED atual
      FastLED.show();                           // Atualiza os LEDs
      fadeall(); // Escurece todos os LEDs gradualmente

      currentLed++; // Avança para o próximo LED

      // Se chegou ao final, inverte a direção
      if (currentLed >= NUM_TOTAL_LEDS) {
        currentLed = NUM_TOTAL_LEDS - 1;
        direction = false; // Inverte a direção
      }
    } else {
      // Movimento da direita para a esquerda
      leds[currentLed] = CHSV(hue++, 255, 255); // Define a cor do LED atual
      FastLED.show();                           // Atualiza os LEDs
      fadeall(); // Escurece todos os LEDs gradualmente

      currentLed--; // Retrocede para o LED anterior

      // Se chegou ao início, inverte a direção
      if (currentLed < 0) {
        currentLed = 0;
        direction = true; // Inverte a direção
      }
    }
  }
}

void Effects::fadeall() {
  for (int i = 0; i < NUM_TOTAL_LEDS; i++) {
    leds[i].nscale8(250);
  }
}

void Effects::colorWipe(CRGB color) { this->colorWipe(color, CRGB::Black, 50); }

void Effects::colorWipe(CRGB color, int time) {
  this->colorWipe(color, CRGB::Black, time);
}

void Effects::colorWipe(CRGB color, CRGB color2, int time) {
  static bool isColor1 = false;
  unsigned long currentMillis = millis();

  if (currentMillis - this->lastUpdate >= time) {
    this->lastUpdate = currentMillis;
    if (isColor1) {
      this->setLedColor(color, currentLed);
    } else {
      this->setLedColor(color2, currentLed);
    }
    currentLed++;
    if (currentLed >= NUM_TOTAL_LEDS) {
      currentLed = 0;
      isColor1 = !isColor1;
    }
  }
}

void Effects::snowSparkle(CRGB color) {
  this->snowSparkle(color, 15, random(100, 1000));
}

void Effects::snowSparkle(CRGB color, int sparkleDelay, int speedDelay) {
  this->fill(color, NUM_TOTAL_LEDS);

  int Pixel = random(NUM_TOTAL_LEDS);
  this->leds[Pixel] = CRGB::Black;
  FastLED.show();
  delay(sparkleDelay);
  this->leds[Pixel] = color;
  FastLED.show();
  delay(speedDelay);
}

void Effects::rainbowCycle() { this->rainbowCycle(20); }

void Effects::rainbowCycle(int speedDelay) {
  uint8_t *c;
  uint16_t j;

  if (millis() - this->lastUpdate > 20) {
    for (int i = 0; i < NUM_TOTAL_LEDS; i++) {
      c = this->wheel(((i * 256 / NUM_TOTAL_LEDS) + j) & 255);
      this->leds[i] = CRGB(*c, *(c + 1), *(c + 2));
    }
    FastLED.show();
    j++;
    this->lastUpdate = millis();
  }
}

uint8_t *Effects::wheel(uint8_t wheelPos) {
  static uint8_t c[3];

  if (wheelPos < 85) {
    c[0] = wheelPos * 3;
    c[1] = 255 - wheelPos * 3;
    c[2] = 0;
  } else if (wheelPos < 170) {
    wheelPos -= 85;
    c[0] = 255 - wheelPos * 3;
    c[1] = 0;
    c[2] = wheelPos * 3;
  } else {
    wheelPos -= 170;
    c[0] = 0;
    c[1] = wheelPos * 3;
    c[2] = 255 - wheelPos * 3;
  }

  return c;
}
