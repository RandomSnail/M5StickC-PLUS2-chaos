# M5-CHAOS: 
# Multi-Tool Security Research Platform

![Version](https://img.shields.io/badge/version-1.1-blueviolet)
![License](https://img.shields.io/badge/license-MIT-green)
![Platform](https://img.shields.io/badge/Hardware-M5StickC%20Plus%202-orange)

**M5-CHAOS** is a comprehensive security research toolkit for the **M5StickC Plus 2**, designed for educational purposes and penetration testing demonstrations. It features wireless protocol testing, sensor utilities, and interactive demonstrations.

---

## Key Features

### Wireless Security Research
* **BLE Proximity Testing** – Bluetooth Low Energy advertisement analysis (iOS/Android/Windows protocols).
* **WiFi Network Simulation** – 802.11 beacon frame generation for network testing.
* **Captive Portal Demo** – Educational demonstration of authentication capture techniques.
* **IR Universal Remote** – Infrared transmitter with 20+ TV brand codes (38-60kHz protocols).

### Sensor Tools
* **Digital Level** – 3-axis accelerometer-based bubble level.
* **Motion Detector** – Gyroscope and accelerometer data visualization.
* **Coin Toss** – Shake-to-flip random number generator using IMU.
* **Precision Timer** – Stopwatch with millisecond accuracy.

---

## Compilation & Installation (Arduino IDE)

To ensure M5-CHAOS compiles and connects correctly, follow these specific settings:

### 1. Prerequisites
* **Board Manager:** Install `esp32` by Espressif Systems (v2.0.x or v3.x).
* **Required Libraries:**
  * `M5Unified` & `M5GFX`
  * `NimBLE-Arduino`
  * `Preferences`, `WiFi`, `WebServer`, `DNSServer` (Built-in)

### 2. IDE Settings
Due to driver requirements and code size, use these settings in the **Tools** menu:
* **Board:** `ESP32 Dev Module` (Matches driver stability)
* **Partition Scheme:** `Huge APP (3MB No OTA/1MB SPIFFS)` — **Required** for BLE/WiFi stacks.
* **Upload Speed:** `115200` (Recommended for connection stability)
* **Core Debug Level:** `None`

### 3. Flashing
1. Place all `.ino` files in a folder named `M5-Chaos`.
2. Open `M5-Chaos.ino`.
3. Connect your M5StickC Plus 2 and click **Upload**.

---

## Web Interface Access

| Parameter | Details |
| :--- | :--- |
| **Default SSID** | `M5-Chaos` |
| **Password** | `m5stack123` |
| **AP URL** | `http://192.168.4.1` |

### Web Dashboard Features
* **Payload List (`/list`)** – View available payloads in JSON.
* **Live Injector (`/type`)** – Send keystrokes via BLE HID.
* **System Stats (`/stats`)** – Real-time Battery, RAM, and Uptime monitoring.
* **WiFi Setup (`/setup`)** – Persistent credential management.

---

## Troubleshooting

### Upload Issues ("Failed to connect to ESP32")
If you see connection errors in the Arduino IDE, try these steps:
1. **The "Boot" Trick:** Hold the **Button G0** (the big button on the front) while clicking Upload. Release it once "Writing at..." appears.
2. **Cable Check:** Ensure you are using a high-quality USB-C **Data** cable. Many charging cables lack the internal data wires.
3. **Driver Reinstall:** If the COM port doesn't appear, reinstall the [CP210x VCP Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) or [CH34x Drivers](https://www.wch-ic.com/downloads/CH341SER_EXE.html).
4. **Baud Rate:** If the upload starts but fails halfway, lower the **Upload Speed** to `115200` in the Tools menu.

### WiFi/BLE Not Working
* **Partition Error:** Ensure you selected the `Huge APP` partition scheme. If the partition is too small, the WiFi/BLE drivers won't initialize, and the device may boot-loop.
* **Camera Test (IR):** To verify the IR emitter is working, point it at your phone's camera while using the IR Remote app. You should see a purple/white flickering light on your phone screen.

### Battery/Power
* If the screen remains black, the battery might be deep-discharged. Plug it into a PC (not a wall charger) for 30 minutes, then hold the power button (side button) for 6 seconds to reset.
---

## Legal & Safety Notice
> [!CAUTION]
> **FOR EDUCATIONAL AND AUTHORIZED TESTING ONLY.**
> Unauthorized use against networks or devices without explicit permission is illegal. The developer assumes no liability for misuse or damages.

---

## License & Credits
* **Author:** Randomsnail
* **Build Date:** 2026-01-16
* **License:** MIT License (Attribution Required)

**Credits:** M5Stack (Hardware), ESP-IDF (Wireless Stack), NimBLE Developers.
