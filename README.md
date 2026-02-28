# Smart Home Controller – ESP32

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE.md)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-blue)](https://platformio.org/)
[![Demo Video](https://img.shields.io/badge/Watch-Demo-red)](LINK_TO_YOUR_VIDEO)

---

## Project Overview

This project is a **Smart Home Controller** built on an **ESP32** microcontroller. It integrates multiple sensors and actuators to deliver a flexible home automation solution with manual, automatic, and remote control capabilities.

Key features include:

- **PIR motion-based lighting** — turns the light off after 2 minutes of inactivity, restores it on motion only if it was previously active, resets the timer on continued motion, and ignores motion if manually switched off.
- **Temperature-based fan control** using a DHT11 sensor.
- **Manual and automatic relay control** for connected devices.
- **Bluetooth Classic support** for offline operation.
- **MQTT integration** for remote monitoring and control.
- **Blynk App interface** for smartphone control.
- **Node-RED dashboard** for visualization and automation flows.
- **Party Mode** — RGB LED light show with configurable patterns.
- **RFID master card** to toggle the system on and off.

---

## Components & Hardware

| Component | Quantity | Notes |
|---|---|---|
| ESP32 Dev Module | 1 | Any ESP32 board |
| Relay Modules | 4 | Controls lights and fan |
| DHT11 Temperature Sensor | 1 | Fan automation |
| PIR Motion Sensor | 1 | Indoor light control |
| RGB LED / WS2812 | 1 | Party Mode |
| HC-SR04 Ultrasonic Sensor | 1 | Optional proximity detection |
| RFID Reader MFRC522 | 1 | Master card control |
| Push Buttons (ADC) | 5 | Manual relay control |
| Buzzer / Piezo | 1 | Feedback and alerts |
| Wires, Breadboard, etc. | — | Prototyping |

Wiring diagrams are available in [`docs/hardware-wiring-diagram.md`](docs/hardware-wiring-diagram.md).

---

## Repository Structure

```
smart-home-esp32/
├─ README.md
├─ LICENSE.md
├─ docs/
│  ├─ deployment-guide.md
│  ├─ mqtt-topic-architecture.md
│  ├─ nodered-dashboard-overview.md
│  └─ media/
│     ├─ images/
│     │  └─ wiring-diagram.png
│     └─ videos/
│        └─ demo.mp4
├─ firmware/
│  ├─ src/
│  │  └─ main.ino
│  ├─ include/
│  │  ├─ config.h
│  │  └─ topics.h
│  └─ lib/
├─ blynk-config/
│  └─ template.json
├─ nodered/
│  └─ flow.json
└─ .gitignore
```

---

## Setup & Deployment

### 1. Firmware

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/smart-home-esp32.git
cd smart-home-esp32/firmware
```

Open `main.ino` (or the `src/` directory) in PlatformIO or Arduino IDE.

Update `config.h` with your credentials:

```cpp
#define WIFI_SSID        "YOUR_WIFI_SSID"
#define WIFI_PASSWORD    "YOUR_WIFI_PASSWORD"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
```

Update `topics.h` with your MQTT broker address and topic structure, then compile and upload to the ESP32.

### 2. Blynk App

Import the template from `blynk-config/template.json`, connect using the same Blynk auth token, and map virtual pins as described in [`docs/deployment-guide.md`](docs/deployment-guide.md).

### 3. Node-RED Dashboard

Import the flow from `nodered/flow.json`, review the layout in [`docs/nodered-dashboard-overview.md`](docs/nodered-dashboard-overview.md), and update the MQTT broker settings to match `topics.h`.

### 4. Media & Demo

Screenshots are located in `docs/media/images/` and demo videos in `docs/media/videos/`. Optional GIFs demonstrating motion detection, party mode, and fan automation can also be placed there.

---

## MQTT Architecture

Full details are available in [`docs/mqtt-topic-architecture.md`](docs/mqtt-topic-architecture.md). Key topics:

| Topic | Purpose |
|---|---|
| `smarthome/esp32/set` | Commands from Node-RED / Blynk |
| `smarthome/esp32/status` | System state updates |
| `smarthome/esp32/event` | Events such as motion, RFID, and party mode |

---

## Usage

The system supports several control modes. Manual control is available through physical buttons or Blynk virtual pins. Automatic control is handled by onboard sensors (PIR, DHT11). Bluetooth commands allow offline operation when Wi-Fi is unavailable. MQTT and Node-RED provide remote monitoring and advanced automation. Party Mode can be triggered via virtual pin `V33` or the MQTT command `PARTY ON`.

---

## License

This project is licensed under the [MIT License](LICENSE.md).

---

## Links

- [Demo Video](LINK_TO_YOUR_VIDEO)
- [Blynk App Template](blynk-config/template.json)
- [Node-RED Flow](nodered/flow.json)
- [Hardware Wiring Diagram](docs/hardware-wiring-diagram.md)

