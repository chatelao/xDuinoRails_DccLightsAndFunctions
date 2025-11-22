#include <Arduino.h>
#undef min
#undef max

// Include FastLED first to define placement new and guards
#include <FastLED.h>

#include <ArduinoSTL.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
