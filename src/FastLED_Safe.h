#ifndef FASTLED_SAFE_H
#define FASTLED_SAFE_H

// Prevent FastLED from defining operator new (conflict with ArduinoSTL)
// We define every known include guard for inplacenew.h
#define __INPLACENEW_H 1
#define FASTLED_INPLACENEW_H 1
#define __INC_INPLACENEW_H 1
#define FL_INPLACENEW_H 1
#define _FL_INPLACENEW_H 1
#define FASTLED_FL_INPLACENEW_H 1
#define __FASTLED_INPLACENEW_H 1
#define INPLACENEW_H 1
#define _INPLACENEW_H_ 1

// Try configuration macros that might disable it
#define FASTLED_NO_PLACEMENT_NEW 1

#include <fastled_config.h>
#include <led_sysdefs.h>
#include <lib8tion.h>

#endif // FASTLED_SAFE_H
