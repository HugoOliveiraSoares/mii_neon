#pragma once
#include "Effects.h"

// Template method implementation for adding strips
template <uint8_t PIN>
void Effects::addStrip(int numLeds) {
    auto strip = std::make_unique<LedStrip<PIN>>(numLeds);
    strips.push_back(std::move(strip));
}