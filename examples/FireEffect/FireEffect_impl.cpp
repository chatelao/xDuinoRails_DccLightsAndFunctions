#include <Arduino.h>
#include <compat/ArduinoSTL_AVR_Compat.h>
#undef min
#undef max
#include <xDuinoRails_DccLightsAndFunctions.h>
#include <cv_definitions.h>
#include <interfaces/ICVAccess.h>
#include <map>
#include <memory>
#include "FireEffect_impl.h"

// Define a pin for the Fire effect
#define FIRE_PIN 6

using namespace xDuinoRails;

// Create an instance of the AuxController
static AuxController controller;

// Mock implementation of ICVAccess for this example
class MockCVAccess : public ICVAccess {
public:
    uint8_t readCV(uint16_t cv) override {
        if (cv_values.count(cv)) {
            return cv_values[cv];
        }
        return 0;
    }

    void writeCV(uint16_t cv, uint8_t value) override {
        cv_values[cv] = value;
    }

private:
    std::map<uint16_t, uint8_t> cv_values;
};

void fire_effect_setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println("Fire Effect Example");

    // Add a physical output (LED) on FIRE_PIN
    controller.addPhysicalOutput(FIRE_PIN, OutputType::LIGHT_SOURCE);

    // Create a mock CV access object and configure it
    MockCVAccess cvAccess;

    // Use the RCN-225 mapping method (CV 96 = 1)
    cvAccess.writeCV(CV_FUNCTION_MAPPING_METHOD, (uint8_t)FunctionMappingMethod::RCN_225);

    // Map Output 1 (Bit 0) to F1.
    // F1 is CV 35.
    // We set bit 0 to map Output 1.
    cvAccess.writeCV(35, 1 << 0);

    // Configure Output 1 to be EFFECT_TYPE_FIRE
    // Access Effects Page
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, EFFECTS_BLOCK_PAGE); // 50

    // Base CV for Output 1 is 257.
    // Offset 0: Type = EFFECT_TYPE_FIRE (8)
    cvAccess.writeCV(257 + EFFECTS_CV_OFFSET_TYPE, EFFECT_TYPE_FIRE);
    // Offset 1,2: Param 1 (Cooling)
    cvAccess.writeCV(257 + EFFECTS_CV_OFFSET_PARAM1_LSB, 55);
    // Offset 3,4: Param 2 (Sparking)
    cvAccess.writeCV(257 + EFFECTS_CV_OFFSET_PARAM2_LSB, 120);
    // Offset 5,6: Param 3 (Length)
    cvAccess.writeCV(257 + EFFECTS_CV_OFFSET_PARAM3_LSB, 3); // 3 virtual pixels

    // Load the configuration from our mock CVs
    controller.loadFromCVs(cvAccess);

    // Initial state: F1 is off
    controller.setFunctionState(1, false);
    Serial.println("Initial state: F1 off.");
}

void fire_effect_loop() {
    // Update the controller
    controller.update(20); // Simulate 20ms passing (50Hz)

    static uint32_t last_toggle = 0;
    static bool f1_state = false;

    // Toggle F1 every 5 seconds
    if (millis() - last_toggle > 5000) {
        last_toggle = millis();
        f1_state = !f1_state;
        controller.setFunctionState(1, f1_state);
        Serial.print("F1 state: ");
        Serial.println(f1_state ? "ON" : "OFF");
    }

    // Small delay to prevent serial spamming if running fast (though in simulation it might not matter)
    delay(20);
}
