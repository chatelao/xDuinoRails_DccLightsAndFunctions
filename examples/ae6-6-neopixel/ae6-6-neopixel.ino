#include <ArduinoSTL.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Fix for conflict between ArduinoSTL and FastLED regarding placement new
#ifndef __INPLACENEW_H
#define __INPLACENEW_H
#endif
#ifndef __INPLACENEW_H__
#define __INPLACENEW_H__
#endif

#include <FastLED.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
