# Agent Instructions for xDuinoRails_DccLightsAndFunctions

This document provides guidance for AI agents working with this codebase.

## Function Mapping

The core logic of this library revolves around DCC Function Mapping, which defines how function key presses on a DCC controller translate to actions on the decoder (e.g., turning on lights, activating effects).

This project adheres to the **RailCommunity (RCN) standards** for function mapping. The relevant specifications are located in the `/docs` directory:

-   **`RCN-225.en.md`**: Defines the standard Configuration Variables (CVs).
-   **`RCN-227.en.md`**: Defines the extended function mapping systems.

### Key Configuration Variable: CV 96

As defined in `RCN-225`, **CV 96** is used to select which function mapping system the decoder should use. The value written to this CV corresponds to one of the methods described in `RCN-227`.

This project supports all function mapping systems described in RCN-227, including both "per-function" and "per-output" methods.

When working on features related to function mapping, always refer to these two documents to ensure compliance with the established standards.
