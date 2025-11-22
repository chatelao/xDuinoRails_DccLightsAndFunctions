#include <Arduino.h>
#undef min
#undef max
#include <ArduinoSTL.h>

// Prevent FastLED from redefining placement new
#ifndef __INPLACENEW_H
#define __INPLACENEW_H
#endif
#ifndef INPLACENEW_H
#define INPLACENEW_H
#endif
#ifndef _INPLACENEW_H
#define _INPLACENEW_H
#endif
#ifndef FASTLED_INPLACENEW_H
#define FASTLED_INPLACENEW_H
#endif
#ifndef FL_INPLACENEW_H
#define FL_INPLACENEW_H
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
