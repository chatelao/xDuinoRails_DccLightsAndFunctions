#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Use compat header for STL (includes ArduinoSTL with patches for AVR)
#include <compat/ArduinoSTL_AVR_Compat.h>

// Include the library main header to ensure dependency detection works
#include <xDuinoRails_DccLightsAndFunctions.h>

// Include safe FastLED header
#include <FastLED_Safe.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
