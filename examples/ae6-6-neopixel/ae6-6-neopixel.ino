#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <xDuinoRails_DccLightsAndFunctions.h>
#include <ArduinoSTL.h>
#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
