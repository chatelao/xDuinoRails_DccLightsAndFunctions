#include <ArduinoSTL.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Use safe FastLED includes to avoid placement new conflict with ArduinoSTL
#include <fastled_config.h>
#include <led_sysdefs.h>
#include <lib8tion.h>

#include "ae6_6_impl.h"

void setup() {
  ae6_6_setup();
}

void loop() {
  ae6_6_loop();
}
