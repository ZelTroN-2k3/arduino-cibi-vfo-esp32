# Arduino Cibi VFO (ESP32 Port)

This project is an adaptation for the **Arduino Nano ESP32** of the original digital VFO by Vincent Hervieux. It is designed to modernize classic CB (Citizen Band) transceivers (such as the President Grant, Superstar 360 FM/3900, etc.) by replacing analog oscillators and channel selectors with high-precision digital synthesis.

## 🚀 Features

- **ESP32 Core**: Ported to Arduino Nano ESP32 for increased power and memory.
- **Si5351 Frequency Synthesis**: Perfect digital stability, replacing old crystals and PLL circuits.
- **OLED Display (128x64)**: Real-time display of frequency, channel, band, modulation mode, and digital S-meter.
- **Multi-mode Support**: Automatic handling of Intermediate Frequencies (IF) for AM, FM, USB, LSB, and CW.
- **Digital Clarifier**: Fine-tune reception via a potentiometer.
- **Band Scan**: Automatic frequency scanning functionality.
- **Non-Volatile Memory**: Automatic saving of the last frequency and settings in the (emulated) EEPROM.
- **Configuration Menu**: Adjust IF offsets, band limits, and calibrate the Si5351 directly using the encoder.

## 🌐 Advanced Web Interface (v2.7)

The project includes a powerful, responsive web interface for remote control and advanced configuration:

- **Professional Display**: High-precision frequency display (00.000.000 MHz) with synchronized Channel and Band indicators.
- **Dual Virtual Knobs**: 
  - **Tuning Knob (Blue)**: High-resolution fine tuning (100 Hz steps).
  - **Channel Knob (Orange)**: Standard CB channel switching (10 kHz steps).
- **Responsive Design**: Optimized for both Desktop and Mobile devices.
- **Interactive S-Meter**: Real-time signal strength visualization synced with the radio.
- **Haptic Feedback**: Vibration support on mobile devices for virtual encoder "clicks" and memory actions.
- **Web-based Advanced Settings (⚙️)**:
  - **Calibration**: Real-time VFO/Si5351 correction adjustment.
  - **IF Offsets**: Configure AM, USB, and LSB intermediate frequencies directly from the browser.
  - **Frequency Limits**: Set custom Min/Max frequency range.
  - **S-Meter Calibration**: Adjust S-Meter sensitivity via software.
  - **Step Increment**: Configure default frequency steps.
- **Customization**: 5 distinct color themes (Classic Green, Vintage Amber, Deep Blue, Arctic White, Emergency Red).
- **Memory Management**: 5 memory slots with quick load, long-press save, and a global "Reset Memories" function.
- **Network Stability**: Built-in request throttling and error handling to ensure ESP32 stability under high usage.

## 🛠 Hardware Parts
- **Board**: Arduino Nano ESP32.
- **Clock Generator**: [Si5351A](doc/Si5351-B.pdf) module (3 outputs).
- **Display**: OLED SSD1306 128x64 IIC.
- **Inputs**: 
  - Rotary encoder with push button.
  - Modulation selector (wired for mode detection).
  - Potentiometers for Clarifier and S-Meter.

## 📐 Principles

### Basic Principles
A rotary encoder with a button is used:
- Turning the encoder adjusts the frequency.
- Clicking the encoder button changes the frequency step or enters digit selection mode.

```mermaid
graph TD;
A[Start]-->B(Setup display);
B-->C(Setup pin);
C-->D(Setup DDS);
D-->E{Loop - Key press interrupt?}
E-->|interrupt|F(Update Display)
F-->G{Freq. changed?}
G-->|No|E
G-->|Yes|H(Update DDS)
H-->E
```

### Advanced Explanations
- **Web Server**: The ESP32 hosts a web server providing a JSON API (`/status`) and control endpoints (`/set`, `/updateconfig`, `/resetmem`).
- **Modulation**: Read from the radio's selector and wired to D8-D12 to adjust VFO and IF (especially for USB/LSB +/- 2.5kHz shifts).
- **TX**: Read from the mic plug (D5) to activate the IF oscillator during transmission only.
- **S-meter**: Implemented via analog input A3.
- **Scan Mode**: Activated in "increment" mode (CH9 switch). When a signal is found (S-meter > threshold), the scan stops.

## 💻 Installation (Arduino IDE)

1.  Download the `arduino-cibi-vfo-esp32` folder.
2.  Open `arduino-cibi-vfo-esp32.ino` in the Arduino IDE.
3.  Install the required library via the Library Manager:
    - **U8g2** (by olikraus).
4.  In **Tools > Board**, select **Arduino Nano ESP32**.
5.  Verify and Upload the code.

*Note: `ClickEncoder` and `Si5351mcu` libraries are included locally in the project folder to ensure compatibility.*

## 🔌 Wiring
Referring to `vfo.h` and `input.cpp`:
- **D12** = INPUT CW
- **D11** = INPUT AM
- **D10** = INPUT FM
- **D9** = INPUT USB
- **D8** = INPUT LSB
- **D6** = CONFIG MENU (active low)
- **D5** = TX (active low)
- **D4** = Button pin (active low)
- **D3** = Rotary encoder A
- **D2** = Rotary encoder B
- **A1** = Step/increment mode (active low)
- **A2** = Clarifier input (analog)
- **A3** = S-meter input (analog)
- **A4** = I2C SDA
- **A5** = I2C SCL

**DDS Output (Si5351):**
- **CLK0** = IF (10.695MHz +/- 2.5kHz)
- **CLK2** = VFO (variable)

## ⚖️ License & Copyright

/*
 * Copyright (c) 2019, Vincent Hervieux vincent.hervieux@gmail.com 
 * https://gitlab.com/croutor/arduino-cibi-vfo
 * Copyright (c) 2026, Patrick Ancher zeltron2k3@gmail.com
 * https://github.com/ZelTroN-2k3/arduino-cibi-vfo
 * 
 * All rights reserved.
 * 
 *
 * This project is distributed under the BSD License.
 */

---
GitHub Repository: [https://github.com/ZelTroN-2k3/arduino-cibi-vfo](https://github.com/ZelTroN-2k3/arduino-cibi-vfo)
