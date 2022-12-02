#pragma once

#include <FastLED.h>
#include "settings.h"

CRGB leds[LED_COUNT];

// Initialize 
void initStatusLeds() {
  FastLED.addLeds<WS2812B, ESP_LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setBrightness(50);
}

void clearLeds() {
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i] = CRGB::Black;
  }
}

// Set the color of LED at position <ledIndex>
void updateLed(uint8_t ledIndex, CRGB led) {
    leds[ledIndex] = led;
    FastLED.show();
}