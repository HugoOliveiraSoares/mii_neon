#include "Effects.h"
#include "EffectErrorCodes.h"
#include "EffectsEnum.h"
#include <Arduino.h>

// Strip management methods
void Effects::initAllStrips() {
  for (auto &strip : strips) {
    strip->init();
  }
}

void Effects::showAllStrips() {
  for (auto &strip : strips) {
    strip->show();
  }
}

void Effects::setColor(CRGB color) { this->currentColor = color; }

CRGB Effects::getCurrentColor() { return this->currentColor; }

void Effects::setHasIndividualColors(int hasIndividualColors) {
  this->hasIndividualColors = hasIndividualColors;
}
bool Effects::isHasIndividualColors() { return this->hasIndividualColors; }

// Basic LED control methods
void Effects::setLedColor(const CRGB &color, int stripIndex, int pos) {
  if (stripIndex >= 0 && stripIndex < strips.size()) {
    strips[stripIndex]->setLedColor(color, pos);
  }
}

void Effects::setSripColor(std::map<int, CRGB> stripColors) {
  for (auto strip : stripColors) {
    this->lastColorPerStrip[strip.first] = strip.second;
  }
}

std::map<int, CRGB> Effects::getLastColorPerStrip() {
  return this->lastColorPerStrip;
}

void Effects::fillStrip(const CRGB &color, int stripIndex) {
  if (stripIndex >= 0 && stripIndex < strips.size()) {
    strips[stripIndex]->fill(color);
  }
}

void Effects::fillAllStrips(const CRGB &color) {
  for (auto &strip : strips) {
    strip->fill(color);
  }
}

int Effects::setBrightness(int bright) {
  if (bright < BRIGHTNESS_MIN_VALUE || bright > BRIGHTNESS_MAX_VALUE) {
    return BRIGHTNESS_OUT_OF_RANGE;
  }

  this->currentBright = bright;
  for (auto &strip : strips) {
    strip->setBrightness(bright);
  }

  return BRIGHTNESS_SUCCESS;
}

int Effects::getBrightness() { return this->currentBright; }

unsigned long Effects::getLastUpdate() const { return this->lastUpdate; }

void Effects::setLastUpdate(unsigned long newUpdate) {
  this->lastUpdate = newUpdate;
}

int Effects::getStripCount() const { return strips.size(); }

int Effects::getStripLedCount(int stripIndex) const {
  if (stripIndex >= 0 && stripIndex < strips.size()) {
    return strips[stripIndex]->getNumTotalLeds();
  }
  return 0;
}

// Blink effects implementation
void Effects::blink(const CRGB &color, const CRGB &color2, int time) {
  if (millis() - lastUpdate > time) {
    if (state.isOn) {
      fillAllStrips(color2);
    } else {
      fillAllStrips(color);
    }
    state.isOn = !state.isOn;
    lastUpdate = millis();

    // Synchronized show across all strips
    showAllStrips();
  }
}

void Effects::blink(const CRGB &color) { blink(color, CRGB::Black, 1000); }

void Effects::blink(const CRGB &color, const CRGB &color2) {
  blink(color, color2, 1000);
}

// ColorWipe effects implementation
void Effects::colorWipe(const CRGB &color, const CRGB &color2, int time,
                        bool isReversed) {
  if (millis() - lastUpdate >= time) {
    lastUpdate = millis();

    for (auto &strip : strips) {
      int maxLeds = strip->getNumTotalLeds();
      if (state.currentLed < maxLeds) {
        CRGB ledColor = state.isOn ? color : color2;
        if (isReversed) {
          strip->setLedColor(ledColor, maxLeds - 1 - state.currentLed);
        } else {
          strip->setLedColor(ledColor, state.currentLed);
        }
      }
    }

    state.currentLed++;

    // Check if all strips are complete
    bool allStripsComplete = true;
    for (auto &strip : strips) {
      if (state.currentLed < strip->getNumTotalLeds()) {
        allStripsComplete = false;
        break;
      }
    }

    if (allStripsComplete) {
      state.currentLed = 0;
      state.isOn = !state.isOn;
    }

    showAllStrips();
  }
}

void Effects::colorWipe(const CRGB &color, int time) {
  colorWipe(color, CRGB::Black, time, false);
}

void Effects::colorWipe(const CRGB &color) {
  colorWipe(color, CRGB::Black, 50, false);
}

void Effects::colorWipeReverse(const CRGB &color) {
  colorWipe(color, CRGB::Black, 50, true);
}

// Cyclon helper methods
void Effects::cyclonUpdatePosition() {
  if (cyclonState.direction) {
    cyclonState.currentLed++;

    // Check if reached max (use 100 as normalized max for scaling)
    if (cyclonState.currentLed >= 100) {
      cyclonState.currentLed = 100;
      cyclonState.direction = false;
    }
  } else {
    cyclonState.currentLed--;

    if (cyclonState.currentLed < 0) {
      cyclonState.currentLed = 0;
      cyclonState.direction = true;
    }
  }
}

void Effects::fadeAllGlobal() {
  for (auto &strip : strips) {
    int numLeds = strip->getNumTotalLeds();
    CRGB *stripLeds = strip->getLeds();

    for (int i = 0; i < numLeds; i++) {
      stripLeds[i].nscale8(250);
    }
  }
}

// Main cyclon effect implementation
void Effects::cyclon() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= CyclonState::UPDATE_INTERVAL) {
    lastUpdate = currentMillis;

    // Process all strips
    for (size_t stripIndex = 0; stripIndex < strips.size(); stripIndex++) {
      int numLeds = strips[stripIndex]->getNumTotalLeds();
      CRGB *stripLeds = strips[stripIndex]->getLeds();

      // Calculate scaled position for this strip's length
      int scaledPosition = map(cyclonState.currentLed, 0, 100, 0, numLeds - 1);

      // Set current LED with shared HSV color
      stripLeds[scaledPosition] = CHSV(cyclonState.hue++, 255, 255);
    }

    // Apply global fade to all strips
    fadeAllGlobal();

    // Update shared movement state
    cyclonUpdatePosition();

    // Synchronized show across all strips
    showAllStrips();
  }
}

/*
 *
 * RainbowCycle Effect
 *
 */

// RainbowCycle helper function - preserve exact wheel algorithm
static CRGB wheel(uint8_t wheelPos) {
  if (wheelPos < 85) {
    return CRGB(wheelPos * 3, 255 - wheelPos * 3, 0);
  } else if (wheelPos < 170) {
    wheelPos -= 85;
    return CRGB(255 - wheelPos * 3, 0, wheelPos * 3);
  } else {
    wheelPos -= 170;
    return CRGB(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}

void Effects::rainbowCycle(int speedDelay) {

  if (millis() - rainbowState.lastUpdate > speedDelay) {
    for (size_t stripIndex = 0; stripIndex < strips.size(); stripIndex++) {
      int numLeds = strips[stripIndex]->getNumTotalLeds();
      CRGB *stripLeds = strips[stripIndex]->getLeds();

      for (int i = 0; i < numLeds; i++) {
        uint8_t wheelPos =
            ((i * 256 / numLeds) + rainbowState.phaseOffset) & 255;
        stripLeds[i] = wheel(wheelPos);
      }
    }

    showAllStrips();
    rainbowState.phaseOffset++;
    rainbowState.lastUpdate = millis();
  }
}

void Effects::rainbowCycle() { rainbowCycle(20); }

/*
 *
 * SnowSparkle Effect
 *
 */

void Effects::snowSparkle(CRGB color) { this->snowSparkle(color, 15); }

void Effects::snowSparkle(CRGB color, int sparkleDelay) {
  this->fillAllStrips(color);

  int strip1 = random(0, strips.size());
  int strip2 = random(0, strips.size());

  int Pixel1 = random(strips[strip1]->getNumTotalLeds());
  int Pixel2 = random(strips[strip2]->getNumTotalLeds());

  this->setLedColor(CRGB::Black, strip1, Pixel1);
  this->setLedColor(CRGB::Black, strip2, Pixel2);
  this->showAllStrips();

  delay(sparkleDelay);

  this->setLedColor(color, strip1, Pixel1);
  this->setLedColor(color, strip2, Pixel2);
  this->showAllStrips();

  delay(random(100, 1000));
}

/*
 *
 * Mii Effect - Different colors for different strips
 *
 */

void Effects::miiEffect() {
  // Define colors for Mii effect
  const CRGB COLOR_PINK = CRGB(255, 0, 255);
  const CRGB COLOR_CYAN = CRGB(0, 0, 255);
  const CRGB COLOR_GRAY = CRGB::White;

  // Strip 0 (Pin 13) -> COLOR_PINK
  // Strip 1 (Pin 14) -> COLOR_PINK
  // Strip 2 (Pin 16) -> COLOR_PINK
  // Strip 3 (Pin 17) -> COLOR_CYAN
  // Strip 4 (Pin 18) -> COLOR_CYAN
  // Strip 5 (Pin 19) -> COLOR_GRAY
  // Strip 6 (Pin 21) -> COLOR_PINK
  // Strip 7 (Pin 22) -> COLOR_GRAY
  // Strip 8 (Pin 23) -> COLOR_GRAY
  // Strip 9 (Pin 25) -> COLOR_GRAY
  // Strip 10 (Pin 26) -> COLOR_PINK
  // Strip 11 (Pin 27) -> COLOR_GRAY

  CRGB stripColors[] = {
      COLOR_PINK, // Pin 13
      COLOR_PINK, // Pin 14
      COLOR_PINK, // Pin 16
      COLOR_CYAN, // Pin 17
      COLOR_CYAN, // Pin 18
      COLOR_GRAY, // Pin 19
      COLOR_PINK, // Pin 21
      COLOR_GRAY, // Pin 22
      COLOR_GRAY, // Pin 23
      COLOR_GRAY, // Pin 25
      COLOR_PINK, // Pin 26
      COLOR_GRAY  // Pin 27
  };

  // Apply colors to each strip
  for (size_t i = 0; i < strips.size() && i < 12; i++) {
    fillStrip(stripColors[i], i);
  }
}

void Effects::estaticEffect() {

  Serial.print("hasIndividualColors: ");
  Serial.println(hasIndividualColors);
  if (this->hasIndividualColors) {

    for (auto lastColor : this->lastColorPerStrip) {
      this->fillStrip(lastColor.second, lastColor.first);
    }
  } else {
    this->fillAllStrips(this->currentColor);
  }
}

EffectsEnum Effects::getCurrentEffect() { return this->currentEffect; }

int Effects::setCurrentEffect(String effectStr) {

  EffectsEnum effect = fromString(effectStr);

  if (effect == static_cast<EffectsEnum>(EFFECT_INVALID_NAME)) {
#ifdef DEV_ENV
    Serial.println("[ERROR] " + String(ERROR_INVALID_NAME_STR) + ": " +
                   effectStr);
    Serial.println("[INFO] Available effects:");
    for (int i = 0; i < EFFECTS_COUNT; i++) {
      EffectsEnum effect = static_cast<EffectsEnum>(i);
      Serial.println("  - " + String(toString(effect)));
    }
#endif

    // Return error code instead of throwing exception to avoid ESP8266 crashes
    return EFFECT_INVALID_NAME;
  }

  this->currentEffect = effect;

  return 0;
}

void Effects::saveConfig() {
  DynamicJsonDocument doc(1024);
  JsonObject ledConfig = doc.createNestedObject("led_config");

  ledConfig["current_effect"] = toString(currentEffect);
  ledConfig["has_individual_colors"] = hasIndividualColors;

  JsonObject globalColor = ledConfig.createNestedObject("global_color");
  globalColor["r"] = currentColor.r;
  globalColor["g"] = currentColor.g;
  globalColor["b"] = currentColor.b;

  JsonArray stripColors = ledConfig.createNestedArray("last_color_per_strip");
  for (auto const &[stripIndex, color] : lastColorPerStrip) {
    JsonObject stripColor = stripColors.createNestedObject();
    stripColor["strip_index"] = stripIndex;
    JsonObject colorObj = stripColor.createNestedObject("color");
    colorObj["r"] = color.r;
    colorObj["g"] = color.g;
    colorObj["b"] = color.b;
  }

  ledConfig["brightness"] = currentBright;

  File f = LittleFS.open("/config.json", "w");
  serializeJson(doc, f);
  f.close();
}

bool Effects::loadConfig() {
  if (!LittleFS.exists("/config.json"))
    return false;

  File f = LittleFS.open("/config.json", "r");
  DynamicJsonDocument doc(1024);
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err)
    return false;

  JsonObject ledConfig = doc["led_config"];

  // Load hasIndividualColors
  hasIndividualColors = ledConfig["has_individual_colors"] | false;

  // Load global color
  if (ledConfig.containsKey("global_color")) {
    JsonObject globalColor = ledConfig["global_color"];
    currentColor = CRGB(globalColor["r"], globalColor["g"], globalColor["b"]);
  }

  // Load lastColorPerStrip
  if (ledConfig.containsKey("last_color_per_strip")) {
    lastColorPerStrip.clear();
    JsonArray stripColors = ledConfig["last_color_per_strip"];
    for (JsonObject stripColor : stripColors) {
      int stripIndex = stripColor["strip_index"];
      JsonObject colorObj = stripColor["color"];
      CRGB color = CRGB(colorObj["r"], colorObj["g"], colorObj["b"]);
      lastColorPerStrip[stripIndex] = color;
    }
  }

  // Load brightness
  currentBright = ledConfig["brightness"] | 200;

  return true;
}
