# User Manual: xDuinoRails_DccLightsAndFunctions

Welcome to the user manual for the xDuinoRails_DccLightsAndFunctions library! This document provides a user-friendly guide to configuring the powerful function mapping features of this library.

## Introduction to Function Mapping

Function mapping is the process of linking a DCC function key (like F1, F2, etc.) to a specific output on your decoder. This output could be a physical pin connected to an LED, a servo, or even a logical action like a sound or a special effect. This library supports several function mapping methods, from the simple to the very complex.

To select which mapping method your decoder should use, you must set **CV 96**. The value you write to this CV determines which of the following mapping systems will be active.

- **CV 96 = 1**: RCN-225 Basic Mapping (CVs 33-46)
- **CV 96 = 2**: RCN-227 Per-Function Mapping
- **CV 96 = 3**: RCN-227 Per-Output Mapping (Version 1)
- **CV 96 = 4**: RCN-227 Per-Output Mapping (Version 2)
- **CV 96 = 5**: RCN-227 Per-Output Mapping (Version 3)

---

## Method 1: Basic RCN-225 Function Mapping (CV 96 = 1)

This is the simplest and most common method for function mapping, directly compatible with the original NMRA DCC standards. It uses CVs 33 through 46 to link functions F0-F12 to physical outputs.

### How it Works

Each CV from 33 to 46 corresponds to a specific DCC function. The value you write to that CV is a bitmask, where each bit represents a physical output. If a bit is set to `1`, the corresponding output will be activated when the DCC function is turned on.

- **Bit 0** corresponds to **Output 1**
- **Bit 1** corresponds to **Output 2**
- **Bit 2** corresponds to **Output 3**
- ...and so on, up to Bit 7 for Output 8.

The CVs are assigned to functions as follows:

| CV | DCC Function |
|----|--------------|
| 33 | F0 (Forward) |
| 34 | F0 (Reverse) |
| 35 | F1           |
| 36 | F2           |
| 37 | F3           |
| 38 | F4           |
| 39 | F5           |
| 40 | F6           |
| 41 | F7           |
| 42 | F8           |
| 43 | F9           |
| 44 | F10          |
| 45 | F11          |
| 46 | F12          |

### Example:

Let's say you want DCC function **F2** to turn on the lights connected to **Output 3** and **Output 4**.

1.  **Identify the CV:** According to the table, F2 is controlled by **CV 36**.
2.  **Determine the Bitmask:**
    - To activate Output 3, you need to set **Bit 2** (value `4`).
    - To activate Output 4, you need to set **Bit 3** (value `8`).
3.  **Calculate the CV Value:** Add the values of the bits together: `4 + 8 = 12`.
4.  **Program the CV:** Write the value `12` to **CV 36**.

Now, whenever you activate F2 on your DCC controller, both Output 3 and Output 4 will turn on.

---

## Method 2: RCN-227 Per-Function Mapping (CV 96 = 2)

This is an advanced mapping system that expands on the basic method. It allows you to map any DCC function (up to F31) to any of the first 24 physical outputs. It also introduces a powerful feature: **blocking functions**.

### How it Works

This method uses a large block of indexed CVs. To access them, you must first set **CV 31 = 0** and **CV 32 = 40**. After that, any reads or writes to CVs 257-512 will access the special mapping memory block.

The memory is organized into 4-byte chunks for each function and direction:

-   **Bytes 1, 2, 3:** A 24-bit bitmask that defines which outputs (1-24) this function will activate.
    -   Byte 1 controls outputs 1-8.
    -   Byte 2 controls outputs 9-16.
    -   Byte 3 controls outputs 17-24.
-   **Byte 4:** The **Blocking Function**. If the DCC function number specified in this byte is **ON**, the current function will be blocked and will not activate its outputs. A value of `255` means no function is blocking it.

The CVs are laid out sequentially:

| Address Range | DCC Function |
|---------------|--------------|
| CV 257-260    | F0 (Forward) |
| CV 261-264    | F0 (Reverse) |
| CV 265-268    | F1 (Forward) |
| CV 269-272    | F1 (Reverse) |
| ...and so on. |              |

### Example:

Let's configure a Mars light. We want **F3** to activate **Output 5**, but only when the locomotive is moving **forward**. We also want to be able to disable the Mars light by turning on **F15**.

1.  **Select the Method:** Program **CV 96 = 2**.
2.  **Access the Indexed CVs:** Program **CV 31 = 0** and **CV 32 = 40**.
3.  **Identify the CVs:** We are configuring **F3 (Forward)**. This corresponds to the CV block for F3, specifically for the forward direction. A little math tells us this is the 8th block (F0 Fwd/Rev, F1 Fwd/Rev, F2 Fwd/Rev, F3 Fwd), starting at CV `257 + (7 * 4) = 285`. So, we will be working with CVs 285-288.
4.  **Determine the Output Mask:** We want to activate **Output 5**. This is bit 4 of the first byte. The value is `2^4 = 16`.
    -   **CV 285 = 16** (Output mask byte 1)
    -   **CV 286 = 0**  (Output mask byte 2)
    -   **CV 287 = 0**  (Output mask byte 3)
5.  **Set the Blocking Function:** We want **F15** to block this function.
    -   **CV 288 = 15** (Blocking function number)

Now, Output 5 will activate when F3 is on and the decoder is moving forward, but only if F15 is off.

---

## Method 3: RCN-227 Per-Output Mapping (V1, V2, V3)

This is the most powerful and flexible set of mapping systems. Instead of configuring which outputs a *function* controls, you configure which functions control an *output*. This approach is often more intuitive for complex lighting schemes. There are three variations of this method.

### Per-Output Version 1: The Matrix (CV 96 = 3)

This method is like a big checklist. For each output, you specify exactly which of the 32 DCC functions should turn it on.

#### How it Works

1.  **Select the Method:** Program **CV 96 = 3**.
2.  **Access the Indexed CVs:** Program **CV 31 = 0** and **CV 32 = 41**.
3.  **Configure:** The memory block is organized with 4 bytes for each output and direction. These 4 bytes are a 32-bit bitmask. Each bit corresponds to a DCC function (F0-F31). If a bit is set, that function will activate the output.
    -   **Byte 1:** Bitmask for functions F0-F7.
    -   **Byte 2:** Bitmask for functions F8-F15.
    -   **Byte 3:** Bitmask for functions F16-F23.
    -   **Byte 4:** Bitmask for functions F24-F31.

#### Example:

You want **Output 2** to be activated by **F1**, **F5**, and **F10** when the locomotive is moving **forward**.

1.  **Identify CVs:** We are configuring **Output 2 (Forward)**. This is the 3rd block in the memory map, starting at CV `257 + (2 * 4) = 265`. We are using CVs 265-268.
2.  **Calculate Bitmasks:**
    -   F1 and F5 are in the first byte. The bitmask is `(1<<1) | (1<<5) = 2 | 32 = 34`.
    -   F10 is in the second byte. The bitmask is `(1<<(10-8)) = 1<<2 = 4`.
3.  **Program CVs:**
    -   **CV 265 = 34** (F0-F7 mask)
    -   **CV 266 = 4** (F8-F15 mask)
    -   **CV 267 = 0**
    -   **CV 268 = 0**

### Per-Output Version 2: Function Numbers (CV 96 = 4)

This method is often easier than calculating bitmasks. Instead of bits, you just write the function numbers that you want to control an output. It also supports **Binary States**, which are conditions internal to the decoder (e.g., "steam generator is on") that are not tied to a DCC function key.

#### How it Works

1.  **Select the Method:** Program **CV 96 = 4**.
2.  **Access the Indexed CVs:** Program **CV 31 = 0** and **CV 32 = 42**.
3.  **Configure:** The memory is organized with 4 bytes per output and direction.
    -   **Bytes 1, 2, 3:** The DCC function numbers that will **activate** the output.
    -   **Byte 4:** The DCC function number that will **block** the output.
    -   A value of `255` means the byte is inactive.
    -   **Binary States:** If you write a value greater than 28, it is interpreted as a Binary State number, not a DCC function.

#### Example:

You want **Output 1** (front headlight) to turn on with **F0** or during shunting mode (**F6**). You also want **F1** to block the light (e.g., to turn it off when a cab light is on). This should only happen in the **forward** direction.

1.  **Identify CVs:** We are configuring **Output 1 (Forward)**, which starts at CV 257.
2.  **Program CVs:**
    -   **CV 257 = 0** (Activating function F0)
    -   **CV 258 = 6** (Activating function F6)
    -   **CV 259 = 255** (Inactive)
    -   **CV 260 = 1** (Blocking function F1)

### Per-Output Version 3: The Ultimate Control (CV 96 = 5)

This is the most advanced system, giving you fine-grained control over direction and allowing for a mix of up to six different activating/blocking conditions per output.

#### How it Works

1.  **Select the Method:** Program **CV 96 = 5**.
2.  **Access the Indexed CVs:** Program **CV 31 = 0** and **CV 32 = 43**.
3.  **Configure:** The memory is organized with **8 bytes per output**. Direction is no longer separate; it's encoded into each byte.
    -   **Bytes 1-4:** These bytes control the output based on simple function triggers. Each byte is encoded as follows:
        -   **Bits 0-5:** The function number (0-63).
        -   **Bits 6-7:** The direction rule:
            -   `00` (value + 0): Activates in any direction.
            -   `01` (value + 64): Activates in **forward** only.
            -   `10` (value + 128): Activates in **reverse** only.
            -   `11` (value + 192): **Blocks** the output.
    -   **Bytes 5-8:** These four bytes form two pairs. Each pair can define a high-number function or a binary state that can also control the output.

#### Example:

Let's configure a modern diesel locomotive's lighting.
-   **Output 1:** Front white light.
-   **Output 2:** Rear red light.

Logic:
-   White light on when moving forward (F0).
-   Red light on when moving forward (F0).
-   Both lights on for shunting mode (F6).
-   F1 blocks the front lights.
-   F2 blocks the rear lights.

**Configuration for Output 1 (Front White Light):**
-   CVs start at 257.
-   **Condition 1:** F0, Forward Only. Value = `0 + 64 = 64`.
-   **Condition 2:** F6, Any Direction. Value = `6 + 0 = 6`.
-   **Condition 3:** F1, Blocking. Value = `1 + 192 = 193`.
-   **Program:** CV 257 = 64, CV 258 = 6, CV 259 = 193, CV 260-264 = 255.

**Configuration for Output 2 (Rear Red Light):**
-   CVs start at `257 + 8 = 265`.
-   **Condition 1:** F0, Forward Only. Value = `0 + 64 = 64`.
-   **Condition 2:** F6, Any Direction. Value = `6 + 0 = 6`.
-   **Condition 3:** F2, Blocking. Value = `2 + 192 = 194`.
-   **Program:** CV 265 = 64, CV 266 = 6, CV 267 = 194, CV 268-272 = 255.

---

## Method 4: Configuring Lighting and Other Effects

Beyond simply turning outputs on and off, this library allows you to configure a wide variety of dynamic effects, from a simple flicker to complex strobe patterns. This is handled by a dedicated block of indexed CVs.

### How it Works

1.  **Access the Indexed CVs:** Program **CV 31 = 0** and **CV 32 = 50**.
2.  **Configure:** The memory block is organized into 8-byte chunks, with one chunk for each of the 24 physical outputs. To find the starting CV for a specific output, use this formula:
    `base_cv = 257 + ( (output_number - 1) * 8 )`

Within each 8-byte block, the CVs have the following roles:

| CV Offset from Base | Name              | Description                                        |
|---------------------|-------------------|----------------------------------------------------|
| +0                  | **Effect Type**   | A number that selects the effect (see table below). |
| +1                  | **Parameter 1 LSB** | Low byte of the first parameter.                     |
| +2                  | **Parameter 1 MSB** | High byte of the first parameter (if needed).      |
| +3                  | **Parameter 2 LSB** | Low byte of the second parameter.                    |
| +4                  | **Parameter 2 MSB** | High byte of the second parameter (if needed).     |
| +5                  | **Parameter 3 LSB** | Low byte of the third parameter.                     |
| +6                  | **Parameter 3 MSB** | High byte of the third parameter (if needed).      |
| +7                  | **Reserved**      | Not currently used.                                |

### Effect Types and Parameters

Here is a list of the available effect types and the parameters they use.

| Effect Name     | Type ID | Parameter 1 (CVs +1, +2) | Parameter 2 (CVs +3, +4) | Parameter 3 (CVs +5, +6) |
|-----------------|---------|--------------------------|--------------------------|--------------------------|
| **Steady**      | 0       | (Unused)                 | (Unused)                 | (Unused)                 |
| **Dimming**     | 1       | Full Brightness (0-255)  | Dimmed Brightness (0-255)| (Unused)                 |
| **Flicker**     | 2       | Base Brightness (0-255)  | Flicker Depth (0-255)    | Flicker Speed (0-255)    |
| **Strobe**      | 3       | Frequency (Hz)           | Duty Cycle (%)           | Brightness (0-255)       |
| **Mars Light**  | 4       | Frequency (mHz)          | Peak Brightness (0-255)  | Phase Shift (%)          |
| **Soft Start**  | 5       | Fade-In Time (ms)        | Fade-Out Time (ms)       | Target Brightness (0-255)|
| **Servo**       | 6       | Endpoint A (angle)       | Endpoint B (angle)       | Travel Speed (1-255)     |
| **Smoke Gen.**  | 7       | Heater (0=off, 1=on)     | Fan Speed (0-255)        | (Unused)                 |

### Example: Configuring a Strobe Light on Output 6

Let's say you want to add a strobe light to your locomotive on **Output 6**. You want it to flash at **5 Hz** with a **30% duty cycle** and full brightness.

1.  **Access the Effects Block:** Program **CV 31 = 0**, **CV 32 = 50**.
2.  **Calculate Base CV:** For Output 6, the base CV is `257 + ((6 - 1) * 8) = 257 + 40 = 297`.
3.  **Program the Effect CVs:**
    -   **CV 297 (Effect Type):** The ID for Strobe is **3**.
    -   **CV 298 (Param 1 LSB):** The frequency is 5 Hz. Since this is less than 256, the LSB is **5** and the MSB is 0.
    -   **CV 299 (Param 1 MSB):** **0**.
    -   **CV 300 (Param 2 LSB):** The duty cycle is 30%. The value is **30**.
    -   **CV 302 (Param 3 LSB):** Full brightness. The value is **255**.

Now, whenever Output 6 is activated by your chosen function mapping method, it will operate as a strobe light with these settings.
