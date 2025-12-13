#include <ArduinoSTL.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <compat/FastLED_Safe.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
