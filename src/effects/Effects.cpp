#include "Effects.h"

// Strip management methods
void Effects::initAllStrips() {
    for (auto& strip : strips) {
        strip->init();
    }
}

void Effects::showAllStrips() {
    for (auto& strip : strips) {
        strip->show();
    }
}

// Basic LED control methods
void Effects::setLedColor(const CRGB &color, int stripIndex, int pos) {
    if (stripIndex >= 0 && stripIndex < strips.size()) {
        strips[stripIndex]->setLedColor(color, pos);
    }
}

void Effects::fillStrip(const CRGB &color, int stripIndex) {
    if (stripIndex >= 0 && stripIndex < strips.size()) {
        strips[stripIndex]->fill(color);
    }
}

void Effects::fillAllStrips(const CRGB &color) {
    for (auto& strip : strips) {
        strip->fill(color);
    }
}

void Effects::fillSegment(const CRGB &color, int stripIndex, Segment segment) {
    if (stripIndex >= 0 && stripIndex < strips.size()) {
        int endPos = segment.start + segment.length;
        int numLeds = strips[stripIndex]->getNumTotalLeds();
        
        // Ensure segment doesn't exceed strip bounds
        if (segment.start < 0 || segment.start >= numLeds) {
            return;
        }
        if (endPos > numLeds) {
            endPos = numLeds;
        }
        
        // Fill the segment one LED at a time
        for (int i = segment.start; i < endPos; i++) {
            strips[stripIndex]->setLedColor(color, i);
        }
    }
}

// Getters
unsigned long Effects::getLastUpdate() const {
    return this->lastUpdate;
}

void Effects::setLastUpdate(unsigned long newUpdate) {
    this->lastUpdate = newUpdate;
}

int Effects::getStripCount() const {
    return strips.size();
}

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

void Effects::blink(const CRGB &color) {
    blink(color, CRGB::Black, 1000);
}

void Effects::blink(const CRGB &color, const CRGB &color2) {
    blink(color, color2, 1000);
}

// ColorWipe effects implementation
void Effects::colorWipe(const CRGB &color, const CRGB &color2, int time) {
    if (millis() - lastUpdate >= time) {
        lastUpdate = millis();
        
        for (auto& strip : strips) {
            int maxLeds = strip->getNumTotalLeds();
            if (state.currentLed < maxLeds) {
                CRGB ledColor = state.isOn ? color : color2;
                strip->setLedColor(ledColor, state.currentLed);
            }
        }
        
        state.currentLed++;
        
        // Check if all strips are complete
        bool allStripsComplete = true;
        for (auto& strip : strips) {
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
    colorWipe(color, CRGB::Black, time);
}

void Effects::colorWipe(const CRGB &color) {
    colorWipe(color, CRGB::Black, 50);
}