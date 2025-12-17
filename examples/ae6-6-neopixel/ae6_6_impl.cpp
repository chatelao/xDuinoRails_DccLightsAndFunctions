#include <Arduino.h>
#undef min
#undef max
#include <xDuinoRails_DccLightsAndFunctions.h>
#include <LightSources/NeopixelRgbMulti.h>
#include <LightSources/NeopixelRgbMultiSwissAe66.h>
#include <cv_definitions.h>
#include <interfaces/ICVAccess.h>
#include "ae6_6_impl.h"

// Define the pins for the Neopixel strips
#define FRONT_LIGHT_PIN 6
#define BACK_LIGHT_PIN 7

using namespace xDuinoRails;

// Create an instance of the AuxController
static AuxController controller;

// Mock implementation of ICVAccess for this example
class MockCVAccess : public ICVAccess {
private:
    struct CVPair {
        uint16_t cv;
        uint8_t value;
    };
    CVPair cv_values[10]; // Store up to 10 CVs for this example
    uint8_t cv_count = 0;

public:
    uint8_t readCV(uint16_t cv) override {
        for (uint8_t i = 0; i < cv_count; ++i) {
            if (cv_values[i].cv == cv) {
                return cv_values[i].value;
            }
        }
        return 0;
    }

    void writeCV(uint16_t cv, uint8_t value) override {
        for (uint8_t i = 0; i < cv_count; ++i) {
            if (cv_values[i].cv == cv) {
                cv_values[i].value = value;
                return;
            }
        }
        if (cv_count < 10) {
            cv_values[cv_count++] = {cv, value};
        }
    }
};

void ae6_6_setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }

    Serial.println("AE6/6 Neopixel Example");

    // Add the light sources to the controller. They get assigned output IDs 0 and 1.
    controller.addLightSource(new NeopixelRgbMulti(FRONT_LIGHT_PIN, 3, 255, 255, 255));
    controller.addLightSource(new NeopixelRgbMultiSwissAe66(BACK_LIGHT_PIN, 3, 255, 0, 0));

    // Create a mock CV access object and configure it
    MockCVAccess cvAccess;

    // Use the RCN-225 mapping method
    cvAccess.writeCV(CV_FUNCTION_MAPPING_METHOD, (uint8_t)FunctionMappingMethod::RCN_225);

    // Map front lights (Output 0) to F0 Forward.
    cvAccess.writeCV(CV_OUTPUT_LOCATION_CONFIG_START, 1 << 0);

    // Map back lights (Output 1) to F0 Reverse.
    cvAccess.writeCV(CV_OUTPUT_LOCATION_CONFIG_START + 1, 1 << 1);

    // Load the configuration from our mock CVs
    controller.loadFromCVs(cvAccess);

    // Initial state: F0 is on, direction is forward
    controller.setFunctionState(0, true);
    controller.setDirection(DECODER_DIRECTION_FORWARD);
    Serial.println("Initial state: F0 on, Direction Forward. Front lights should be on.");
}

void ae6_6_loop() {
    controller.update(100);

    static unsigned long last_change = 0;
    static int state = 0;

    if (millis() - last_change > 5000) {
        last_change = millis();
        state = (state + 1) % 4;

        switch (state) {
            case 0:
                controller.setFunctionState(0, true);
                controller.setDirection(DECODER_DIRECTION_FORWARD);
                Serial.println("State 0: F0 on, Direction Forward. Front lights should be on.");
                break;
            case 1:
                controller.setDirection(DECODER_DIRECTION_REVERSE);
                Serial.println("State 1: Direction changed to Reverse. Back lights should be on, front lights off.");
                break;
            case 2:
                controller.setFunctionState(0, false);
                Serial.println("State 2: F0 turned off. All lights should be off.");
                break;
            case 3:
                controller.setFunctionState(0, true);
                controller.setDirection(DECODER_DIRECTION_FORWARD);
                Serial.println("State 3: F0 turned on, Direction changed to Forward. Front lights should be on.");
                break;
        }
    }
}
