# xDuinoRails_DccLightsAndFunctions

[![Build Status](https://github.com/your-username/your-repo/actions/workflows/build.yml/badge.svg)](https://github.com/your-username/your-repo/actions/workflows/build.yml)

A powerful and flexible Arduino library for controlling DCC auxiliary functions, lighting, and other special effects.

This library provides a comprehensive system for managing physical outputs (like LEDs and servos), logical functions, and complex CV-based function mapping for custom DCC decoders. It is designed to be highly configurable and compliant with the latest RailCommunity (RCN) standards.

## Features

-   **Multiple Function Mapping Systems:** Supports a wide range of mapping methods, from the classic RCN-225 (CVs 33-46) to the highly advanced RCN-227 "per-output" systems.
-   **Configurable Lighting Effects:** Go beyond simple on/off control with a variety of built-in, configurable effects:
    -   Dimming
    -   Flicker (e.g., for fireboxes or lanterns)
    -   Strobe and Mars lights
    -   Soft start/stop for incandescent bulb simulation
    -   ...and more!
-   **Servo Control:** Drive up to 8 servos for animations like moving pantographs, opening doors, or uncouplers.
-   **Extensible and Object-Oriented:** The library is designed to be easily extended with custom light sources (e.g., NeoPixel strips) and new effects.
-   **Compliant with Standards:** Adheres to the RCN-225 and RCN-227 specifications for function mapping, ensuring compatibility and interoperability.

## Installation

1.  **Download the Library:** Download the latest release from the GitHub repository.
2.  **Install in Arduino IDE:**
    -   Open the Arduino IDE.
    -   Go to `Sketch` -> `Include Library` -> `Add .ZIP Library...`.
    -   Select the downloaded ZIP file.
3.  **Install Dependencies:** This library requires the following other Arduino libraries. You can install them through the Arduino Library Manager (`Sketch` -> `Include Library` -> `Manage Libraries...`):
    -   `Servo`
    -   `Adafruit NeoPixel`
    -   `ArduinoSTL`

## Basic Usage

To use the library, you'll need to include the main header and create an instance of the `AuxController` class.

```cpp
#include <xDuinoRails_DccLightsAndFunctions.h>

xDuinoRails::AuxController auxController;

void setup() {
  // Add your physical outputs
  // This adds a simple LED on pin 13
  auxController.addPhysicalOutput(13, xDuinoRails::OutputType::LIGHT_SOURCE);

  // You would typically load the configuration from your decoder's CVs here
  // For example:
  // MyCVAccessor cvAccessor; // Your implementation of the ICVAccess interface
  // auxController.loadFromCVs(cvAccessor);
}

void loop() {
  // Update the controller in your main loop
  // Pass the time elapsed since the last update in milliseconds
  auxController.update(10); // Example: 10ms elapsed
  delay(10);
}

// You will also need to provide functions to update the decoder's state
// For example, when a DCC function packet is received:
void onDccFunctionPacket(uint8_t functionNumber, bool functionState) {
  auxController.setFunctionState(functionNumber, functionState);
}
```

## Configuration

All configuration of function mapping and lighting effects is done through Configuration Variables (CVs), just like a commercial DCC decoder.

For a detailed guide on how to configure the different function mapping methods and lighting effects, please refer to the **[User Manual](docs/USER_MANUAL.md)**.

## Dependencies

-   [Arduino Servo Library](https://www.arduino.cc/reference/en/libraries/servo/)
-   [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)
-   [ArduinoSTL](https://github.com/mike-matera/ArduinoSTL)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
