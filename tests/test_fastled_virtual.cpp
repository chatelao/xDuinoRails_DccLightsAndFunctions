#include <FastLED.h>
#include <iostream>

// Mocking Arduino/FastLED dependencies if needed, but FastLED is usually self-contained for math.
// However, FastLED depends on Arduino.h. We need to mock it or use a platform that supports it.
// Since I don't have the full Arduino environment here, I will assume standard behavior:
// FastLED math functions (hsv2rgb, etc) work on generic C++ if headers are right.
// But FastLED headers are heavily Arduino-centric.

// Let's see if I can compile a simple "include FastLED" with my existing mocks.

int main() {
    // Define a virtual strip
    CRGB leds[5];

    // Use a FastLED math function
    // fill_rainbow(leds, 5, 0, 7); // checks if this links

    // Since I cannot easily link the real FastLED library here (it's not installed in the environment),
    // I have to rely on my knowledge or a quick search.

    // However, the user's question is "Is it possible".
    // The answer is definitely Yes.

    return 0;
}
