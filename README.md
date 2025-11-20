# xDuinoRails_DccLightsAndFunctions

This repository contains an Arduino library for controlling DCC auxiliary functions and lighting effects.

This powerful and flexible library is designed for model railroad hobbyists and developers who want to create advanced lighting and function effects on their DCC-equipped locomotives and rolling stock. It fully supports the RailCommunity (RCN) standards for function mapping, including RCN-225 and RCN-227, providing a wide range of configuration options from basic to expert level.

## Features

*   **Multiple Function Mapping Systems:** Supports all standard DCC function mapping methods as defined by the RCN, selected via CV 96:
    *   **RCN-225 (CV 96 = 1):** Standard mapping using CVs 33-46.
    *   **RCN-227 "Per-Function" (CV 96 = 2):** Extended mapping with blocking functions.
    *   **RCN-227 "Per-Output" (CV 96 = 3, 4, 5):** The most flexible and recommended methods for complex lighting logic.
*   **Advanced Lighting Effects:** Configure a wide array of dynamic lighting effects for each output, including:
    *   Dimming and Soft Start/Stop
    *   Flicker, Strobe, and Mars Lights
    *   And more...
*   **Servo Control:** Drive servo motors for animations.
*   **Extensible:** The library is designed to be easily extensible with new light sources and effects.

## Getting Started

To learn how to use this library and configure its many features, please refer to the detailed **[User Manual](docs/USER_MANUAL.md)**.

## Contributing

Contributions are welcome! If you would like to contribute to the development of this library, please feel free to fork the repository and submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
