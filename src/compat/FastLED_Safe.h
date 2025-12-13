#ifndef FASTLED_SAFE_H
#define FASTLED_SAFE_H

// Define guards to prevent FastLED from defining operator new (conflict with ArduinoSTL)
// Different FastLED versions use different guards, so we define all common ones.
#define __INPLACENEW_H
#define FASTLED_INPLACENEW_H
#define __INC_INPLACENEW_H
#define FL_INPLACENEW_H
#define FASTLED_SRC_FL_INPLACENEW_H

#include <fastled_config.h>
#include <led_sysdefs.h>
#include <lib8tion.h>

#endif // FASTLED_SAFE_H
