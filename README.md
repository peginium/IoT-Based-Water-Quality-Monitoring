# 💧 Smart Water Quality Monitoring System (SWQMS)

![C](https://img.shields.io/badge/Language-C-blue.svg)
![Microcontroller](https://img.shields.io/badge/MCU-MSP430G2553-red.svg)
![Protocol](https://img.shields.io/badge/Protocol-I2C%20%7C%20UART%20%7C%201--Wire-brightgreen.svg)

An end-to-end, IoT-based Smart Water Quality Monitoring Station designed for real-time, in-situ analysis of water health. The system utilizes edge-processing to calculate a universal **Water Quality Index (WQI)** based on multiple sensor readings and streams JSON-formatted data wirelessly to a custom web dashboard.

Developed as a functional hardware-software integration project by **Chipware Labs**.

## ✨ Key Features
* **Real-Time Data Acquisition:** Simultaneously measures Temperature (°C), Total Dissolved Solids (TDS in ppm), and pH levels.
* **Edge Computing:** Performs hardware-level temperature compensation and WQI calculation directly on the MSP430 MCU.
* **Dual Visualization:** * *Local:* 0.96" I2C OLED display for immediate on-site readings.
  * *Remote:* Wireless transmission to a browser-based dashboard.
* **Web Serial API Dashboard:** Zero-server architecture. Connects directly to the HC-05 Bluetooth module via the browser to parse and display real-time JSON data.
* **Cascade Power Management:** Custom power distribution using LM7805 (5V) and LM1117T (3.3V) to ensure thermal stability and sensor accuracy.

## 🛠️ Hardware Architecture

### Components Used
* **MCU:** Texas Instruments MSP430G2553
* **Temperature Sensor:** DS18B20 (Waterproof, 1-Wire)
* **TDS Sensor:** Analog TDS Meter
* **pH Sensor:** DFRobot Gravity Analog pH Meter V2
* **Display:** Waveshare 0.96" OLED (I2C Mode)
* **Wireless Module:** HC-05 Bluetooth (UART)
* **Power:** 9V Battery + LM7805 & LM1117T-3.3 Regulators

### Pinout & Wiring Connections
| Module / Sensor | Pin / Function | MSP430G2553 Pin | Notes |
| :--- | :--- | :--- | :--- |
| **DS18B20** | DQ (Data) | `P2.0` | Requires a 4.7kΩ pull-up resistor to 3.3V. |
| **Analog TDS** | A0 (Signal) | `P1.4` (A4) | ADC input. Needs temperature compensation. |
| **Gravity pH V2** | A (Signal) | `P1.5` (A5) | ADC input. |
| **OLED Display**| SCL (Clock) | `P1.6` | I2C SCL. Requires 4.7kΩ pull-up resistor. |
| **OLED Display**| SDA (Data) | `P1.7` | I2C SDA. Requires 4.7kΩ pull-up resistor. |
| **HC-05** | TXD | `P1.1` (RXD) | Cross-connection for UART. |
| **HC-05** | RXD | `P1.2` (TXD) | Cross-connection for UART. |

> **⚠️ Important Hardware Note:** Ensure that the OLED's `CS` and `DC` pins are grounded to lock it into I2C mode (Address: `0x3C`), and tie the `RES` pin to 3.3V.

## 💻 Software Architecture
The embedded firmware is written in **C** using **Code Composer Studio (CCS)**. It focuses on lightweight operations to fit within the 16KB flash memory limit of the MSP430G2553.

* **Sensor Calibration:** Linear interpolation is used for pH, and active temperature compensation formula is applied to TDS readings.
* **WQI Algorithm:** A weighted arithmetic mean algorithm converts raw parameters into a single score (0-100) and categorizes water into *Excellent, Good, Poor, or Undrinkable*.
* **JSON Formatting:** To prevent memory overflow, custom string parsing algorithms are implemented instead of standard `sprintf` libraries. Data is packaged as `{"t":25.5,"tds":145,"ph":7.1,"wqi":85}\r\n`.

## 🚀 Installation & Setup

### 1. Embedded Firmware
1. Clone this repository: `git clone https://github.com/peginium/IoT-Based-Water-Quality-Monitoring.git`
2. Open **Code Composer Studio (CCS)** and import the project.
3. Build the project and flash it to the MSP430 Launchpad.
4. Wire the components according to the table above.

### 2. Web Dashboard
1. Power up the hardware system.
2. Pair your computer with the **HC-05** Bluetooth module (Default PIN is usually `1234` or `0000`).
3. Open the `index.html` file (located in the `/web` folder of this repo) using Google Chrome or Microsoft Edge.
4. Click the **Connect** button on the web page and select the Bluetooth COM port from the browser prompt.
5. Watch the real-time data flow!

## 📸 System Previews
*(Upload pictures of your breadboard circuit, the OLED display working, and a screenshot of the Web Dashboard here, and replace the placeholder links below)*
* `![Hardware Setup](link_to_hardware_image.jpg)`
* `![Web Dashboard](link_to_dashboard_image.jpg)`

## 👨‍💻 Author & Credits
* **Arhan Özcan** - Embedded System Design & Prototyping
* **Chipware Labs** - Teknofest Project Development

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).
