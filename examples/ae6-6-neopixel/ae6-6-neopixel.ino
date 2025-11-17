#include <xDuinoRails_DccLightsAndFunctions.h>
#include <LightSources/NeopixelRgbMulti.h>
#include <LightSources/NeopixelRgbMultiSwissAe66.h>
#include <interfaces/ICVAccess.h>
#include <map>

// Define the pins for the Neopixel strips
#define FRONT_LIGHT_PIN 6
#define BACK_LIGHT_PIN 7

using namespace xDuinoRails;

// Create an instance of the AuxController
AuxController controller;

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

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB
    }

    Serial.println("AE6/6 Neopixel Example");

    // Create the light sources for the front and back lights (3 pixels each)
    // Front lights are white
    auto frontLights = std::make_unique<NeopixelRgbMulti>(FRONT_LIGHT_PIN, 3, 255, 255, 255);
    // Back lights are red
    auto backLights = std::make_unique<NeopixelRgbMultiSwissAe66>(BACK_LIGHT_PIN, 3, 255, 0, 0);

    // Add the light sources to the controller. They get assigned output IDs 0 and 1.
    controller.addLightSource(std::move(frontLights));
    controller.addLightSource(std::move(backLights));

    // Create a mock CV access object and configure it
    MockCVAccess cvAccess;

    // Use the RCN-225 mapping method
    cvAccess.writeCV(CV_FUNCTION_MAPPING_METHOD, (uint8_t)FunctionMappingMethod::RCN_225);

    // Map front lights (Output 0) to F0 Forward.
    // CV 33 (CV_OUTPUT_LOCATION_CONFIG_START) controls F0f.
    // We set bit 0 to map Output 0.
    cvAccess.writeCV(CV_OUTPUT_LOCATION_CONFIG_START, 1 << 0);

    // Map back lights (Output 1) to F0 Reverse.
    // CV 34 (CV_OUTPUT_LOCATION_CONFIG_START + 1) controls F0b.
    // We set bit 1 to map Output 1.
    cvAccess.writeCV(CV_OUTPUT_LOCATION_CONFIG_START + 1, 1 << 1);

    // Load the configuration from our mock CVs
    controller.loadFromCVs(cvAccess);

    // Initial state: F0 is on, direction is forward
    controller.setFunctionState(0, true);
    controller.setDirection(DECODER_DIRECTION_FORWARD);
    Serial.println("Initial state: F0 on, Direction Forward. Front lights should be on.");
}

void loop() {
    // Update the controller
    controller.update(100); // Simulate 100ms passing

    // After 5 seconds, change direction to reverse
    delay(5000);
    controller.setDirection(DECODER_DIRECTION_REVERSE);
    Serial.println("Direction changed to Reverse. Back lights should be on, front lights off.");

    // Update the controller
    controller.update(100);

    // After 5 seconds, turn F0 off
    delay(5000);
    controller.setFunctionState(0, false);
    Serial.println("F0 turned off. All lights should be off.");

    // Update the controller
    controller.update(100);

    // After 5 seconds, turn F0 on and change direction to forward
    delay(5000);
    controller.setFunctionState(0, true);
    controller.setDirection(DECODER_DIRECTION_FORWARD);
    Serial.println("F0 turned on, Direction changed to Forward. Front lights should be on.");
}
