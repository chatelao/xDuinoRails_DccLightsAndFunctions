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

To calculate the value to write into the CV, use the table below. Simply find the outputs you want to turn on and add their **Values** together.

#### Bit Value Conversion Table

| Output # | Bit # | Value |
| :---: | :---: | :---: |
| 1 | 0 | **1** |
| 2 | 1 | **2** |
| 3 | 2 | **4** |
| 4 | 3 | **8** |
| 5 | 4 | **16** |
| 6 | 5 | **32** |
| 7 | 6 | **64** |
| 8 | 7 | **128** |

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
    - To activate Output 3, look at the table above: Output 3 corresponds to Value **4**.
    - To activate Output 4, look at the table above: Output 4 corresponds to Value **8**.
3.  **Calculate the CV Value:** Add the values together: `4 + 8 = 12`.
4.  **Program the CV:** Write the value `12` to **CV 36**.

Now, whenever you activate F2 on your DCC controller, both Output 3 and Output 4 will turn on.

---

## Method 2: RCN-227 Per-Function Mapping (CV 96 = 2)

This is an advanced mapping system that expands on the basic method. It allows you to map any DCC function (up to F31) to any of the first 24 physical outputs. It also introduces a powerful feature: **blocking functions**.

### How it Works

This method uses a large block of indexed CVs. To access them, you must first set **CV 31 = 0** and **CV 32 = 40**. After that, any reads or writes to CVs 257-512 will access the special mapping memory block.

The memory is organized into 4-byte chunks for each function and direction:

-   **Bytes 1, 2, 3:** A 24-bit bitmask that defines which outputs (1-24) this function will activate.
-   **Byte 4:** The **Blocking Function**. If the DCC function number specified in this byte is **ON**, the current function will be blocked and will not activate its outputs. A value of `255` means no function is blocking it.

#### Output Mask Table

Use this table to calculate the value for each of the first 3 bytes.

| Bit | Value | Byte 1 (Outputs 1-8) | Byte 2 (Outputs 9-16) | Byte 3 (Outputs 17-24) |
| :---: | :---: | :---: | :---: | :---: |
| 0 | **1** | Output 1 | Output 9 | Output 17 |
| 1 | **2** | Output 2 | Output 10 | Output 18 |
| 2 | **4** | Output 3 | Output 11 | Output 19 |
| 3 | **8** | Output 4 | Output 12 | Output 20 |
| 4 | **16** | Output 5 | Output 13 | Output 21 |
| 5 | **32** | Output 6 | Output 14 | Output 22 |
| 6 | **64** | Output 7 | Output 15 | Output 23 |
| 7 | **128** | Output 8 | Output 16 | Output 24 |

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
4.  **Determine the Output Mask:** We want to activate **Output 5**. Looking at the table, Output 5 is in **Byte 1** and has a value of **16**.
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
3.  **Configure:** The memory block is organized with 4 bytes for each output and direction. Each bit corresponds to a DCC function (F0-F31).

#### Function Bitmask Table

Use this table to calculate the value for each byte. Find the function you want, note its Value, and add it to the total for that byte.

| Bit | Value | Byte 1 (F0-F7) | Byte 2 (F8-F15) | Byte 3 (F16-F23) | Byte 4 (F24-F31) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| 0 | **1** | F0 | F8 | F16 | F24 |
| 1 | **2** | F1 | F9 | F17 | F25 |
| 2 | **4** | F2 | F10 | F18 | F26 |
| 3 | **8** | F3 | F11 | F19 | F27 |
| 4 | **16** | F4 | F12 | F20 | F28 |
| 5 | **32** | F5 | F13 | F21 | F29 |
| 6 | **64** | F6 | F14 | F22 | F30 |
| 7 | **128** | F7 | F15 | F23 | F31 |

#### Example:

You want **Output 2** to be activated by **F1**, **F5**, and **F10** when the locomotive is moving **forward**.

1.  **Identify CVs:** We are configuring **Output 2 (Forward)**. This is the 3rd block in the memory map, starting at CV `257 + (2 * 4) = 265`. We are using CVs 265-268.
2.  **Calculate Bitmasks:**
    -   **Byte 1 (F0-F7):** We need F1 (Value 2) and F5 (Value 32). `2 + 32 = 34`.
    -   **Byte 2 (F8-F15):** We need F10 (Value 4). `4`.
3.  **Program CVs:**
    -   **CV 265 = 34**
    -   **CV 266 = 4**
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

#### Control Byte Encoding Table

Each byte contains both the function number and its rule (Direction or Blocking). Add the **Base Value** for the rule to the **Function Number** to get the final CV value.

| Rule | Base Value | Valid Range | Description |
| :--- | :--- | :--- | :--- |
| **Multi-Direction** | **0** | 0 - 63 | Function activates the output in **Any** direction. |
| **Forward Only** | **64** | 64 - 127 | Function activates the output in **Forward** only. |
| **Reverse Only** | **128** | 128 - 191 | Function activates the output in **Reverse** only. |
| **Blocking** | **192** | 192 - 255 | Function **Blocks** the output (Any direction). |

**Bytes 1-4:** Primary triggers using the encoding above.
**Bytes 5-8:** Extended triggers (pairs of bytes for high function numbers/binary states).

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
-   **Condition 1:** F0, Forward Only. Base 64 + Func 0 = **64**.
-   **Condition 2:** F6, Any Direction. Base 0 + Func 6 = **6**.
-   **Condition 3:** F1, Blocking. Base 192 + Func 1 = **193**.
-   **Program:** CV 257 = 64, CV 258 = 6, CV 259 = 193, CV 260-264 = 255.

**Configuration for Output 2 (Rear Red Light):**
-   CVs start at `257 + 8 = 265`.
-   **Condition 1:** F0, Forward Only. Base 64 + Func 0 = **64**.
-   **Condition 2:** F6, Any Direction. Base 0 + Func 6 = **6**.
-   **Condition 3:** F2, Blocking. Base 192 + Func 2 = **194**.
-   **Program:** CV 265 = 64, CV 266 = 6, CV 267 = 194, CV 268-272 = 255.

---

## Method 4: Configuring Lighting and Other Effects

Beyond simply turning outputs on and off, this library allows you to configure a wide variety of dynamic effects, from a simple flicker to complex strobe patterns. This is handled by a dedicated block of indexed CVs.

### How it Works

1.  **Access the Indexed CVs:** Program **CV 31 = 0** and **CV 32 = 50**.
2.  **Configure:** The memory block is organized into 8-byte chunks, with one chunk for each of the 24 physical outputs.

#### Quick Reference: Output Base CVs

Use this table to quickly find the starting CV for any output.

| Output | Base CV | Output | Base CV | Output | Base CV |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **1** | 257 | **9** | 321 | **17** | 385 |
| **2** | 265 | **10** | 329 | **18** | 393 |
| **3** | 273 | **11** | 337 | **19** | 401 |
| **4** | 281 | **12** | 345 | **20** | 409 |
| **5** | 289 | **13** | 353 | **21** | 417 |
| **6** | 297 | **14** | 361 | **22** | 425 |
| **7** | 305 | **15** | 369 | **23** | 433 |
| **8** | 313 | **16** | 377 | **24** | 441 |

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
2.  **Calculate Base CV:** Looking at the *Quick Reference* table, the Base CV for Output 6 is **297**.
3.  **Program the Effect CVs:**
    -   **CV 297 (Effect Type):** The ID for Strobe is **3**.
    -   **CV 298 (Param 1 LSB):** The frequency is 5 Hz. Since this is less than 256, the LSB is **5** and the MSB is 0.
    -   **CV 299 (Param 1 MSB):** **0**.
    -   **CV 300 (Param 2 LSB):** The duty cycle is 30%. The value is **30**.
    -   **CV 302 (Param 3 LSB):** Full brightness. The value is **255**.

Now, whenever Output 6 is activated by your chosen function mapping method, it will operate as a strobe light with these settings.
