#include <Arduino.h>
#undef min
#undef max

// Include main library header to trigger library discovery and include path setup.
// This header transitively includes 'compat/ArduinoSTL_AVR_Compat.h',
// which handles FastLED/ArduinoSTL conflicts and unique_ptr polyfills.
#include <xDuinoRails_DccLightsAndFunctions.h>

#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
