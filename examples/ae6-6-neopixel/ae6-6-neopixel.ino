#include <Arduino.h>
#undef min
#undef max

// Include the compatibility header which handles FastLED/ArduinoSTL conflicts
// and provides unique_ptr polyfills.
#include <compat/ArduinoSTL_AVR_Compat.h>

#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
