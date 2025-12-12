#include <ArduinoSTL.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Extensive guards for FastLED placement new to prevent conflict with ArduinoSTL
#ifndef __INPLACENEW_H
#define __INPLACENEW_H
#endif
#ifndef __INPLACENEW_H__
#define __INPLACENEW_H__
#endif
#define _NEW
#define _NEW_
#define __NEW
#define __NEW__
#define FASTLED_INPLACENEW_H

#include <FastLED.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
