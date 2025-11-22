#include <Arduino.h>
#undef min
#undef max
#include <ArduinoSTL.h>

// Prevent FastLED from redefining placement new
// This guard stops FastLED/src/fl/inplacenew.h from defining operator new
#ifndef __INPLACENEW_H
#define __INPLACENEW_H
#endif

#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
