

# Deployment Guide
### Smart IoT Automation System

This document covers the full deployment process for the system, including ESP32 firmware configuration, Blynk IoT integration, MQTT setup via Mosquitto and Node-RED, Bluetooth Low Energy control, Wi-Fi configuration, sensor calibration, and automation logic.

---

## 1. Hardware Requirements

| Component | Purpose |
|---|---|
| ESP32 | Main controller |
| DHT11 / DHT22 | Temperature and humidity sensing |
| LDR | Light level detection |
| PIR Sensor | Motion detection |
| HC-SR04 | Distance measurement |
| MFRC522 RFID | Master system switch |
| Relay Module | Fan and load control |
| Active Buzzer | Alerts and feedback |
| Push Buttons | Mode control |
| RGB LED | Status indication |

---

## 2. Software Requirements

The following software and libraries are required before proceeding:

- Arduino IDE or PlatformIO
- ESP32 Board Package
- Blynk IoT Library
- PubSubClient (MQTT)
- MFRC522 Library
- DHT Sensor Library
- Wi-Fi Library (included with ESP32 package)

---

## 3. Configuration

Before flashing the firmware, open `include/config.h` and update all credentials and settings:

```cpp
// Wi-Fi
#define WIFI_SSID        "YOUR_WIFI_SSID"
#define WIFI_PASS        "YOUR_WIFI_PASSWORD"

// Blynk
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

// MQTT — use the local IP of the machine running Mosquitto
#define MQTT_SERVER "192.168.1.100"
#define MQTT_PORT   1883
```

---

## 4. Flashing the ESP32

Connect the ESP32 via USB, select **ESP32 Dev Module** as the board, choose the correct COM port, and click **Upload**. Once flashed, the serial monitor should confirm the following connection sequence:

```
WiFi connected
Blynk connected
MQTT connected
System Ready
```

---

## 5. Blynk App Setup

In the Blynk IoT Console, create a new template and configure the following virtual pins:

| Function | Virtual Pin |
|---|---|
| Temperature | V0 |
| Humidity | V1 |
| Light Level | V2 |
| Distance | V3 |
| Fan Control | V4 |
| System Status | V5 |
| Mode Select | V6 |

Add the following widgets to the mobile dashboard: Gauge (Temperature), Gauge (Humidity), LED (Motion), Switch (Fan), and Chart (Data Logging).

---

## 6. MQTT Setup (Mosquitto)

Start the Mosquitto broker before powering the ESP32.

On Windows:

```bash
mosquitto -v
```

On Linux:

```bash
sudo systemctl start mosquitto
```

---

## 7. Node-RED Setup

Open Node-RED at `http://localhost:1880` and build a flow with the following nodes: MQTT In, MQTT Out, Debug, and your preferred Dashboard UI elements.

Subscribe to the following topics:

```
iot/home/temperature
iot/home/humidity
iot/home/light
iot/home/distance
iot/home/system
```

---

## 8. BLE Setup

Using any BLE scanner app, scan for a device named `ESP32_IoT_System` and connect. The following commands are supported:

| Command | Function |
|---|---|
| `FAN_ON` | Turn fan on |
| `FAN_OFF` | Turn fan off |
| `STATUS` | Request system status |
| `SYSTEM_OFF` | Master shutdown |
| `SYSTEM_ON` | Master enable |

---

## 9. RFID Master Switch

The authorized card UID is `E9 A3 4D 06`. Tapping the card once enables the system; tapping again disables it. When disabled, all sensors and outputs are deactivated.

---

## 10. Automation Logic

**Motion** — The PIR sensor triggers the buzzer with a 1-minute cooldown between activations.

**Temperature** — The fan turns on automatically when temperature exceeds the defined threshold. Manual override is available via Blynk or BLE.

**Light** — The LDR sensor controls RGB LED brightness, adjusted for ambient conditions.

**Distance** — If an object is detected closer than 5 cm, a buzzer alert is triggered.

---

## 11. Data Logging

Sensor data is logged to Blynk Cloud automatically. MQTT publishes the same data to Node-RED, which can forward it to a CSV file, a database, or a live dashboard chart.

---

## 12. FreeRTOS Architecture

The firmware uses a non-blocking, multi-core architecture built on FreeRTOS:

| Task | Core |
|---|---|
| Sensor Task | Core 1 |
| MQTT Task | Core 0 |
| Blynk Task | Core 0 |
| BLE Task | Core 1 |
| Automation Task | Core 1 |

Timing is managed through `millis()`, task scheduling, and FreeRTOS queues to avoid blocking the main loop.

---

## 13. System Test Checklist

Before finalizing deployment, verify the following:

- Wi-Fi connects successfully
- Blynk dashboard updates in real time
- MQTT publishes sensor data to the broker
- BLE responds to all commands
- RFID card enables and disables the system
- PIR cooldown timer functions correctly
- Distance alert triggers below 5 cm
- Buttons trigger the buzzer as expected
- RGB brightness is controlled by the LDR

---

## 14. Production Recommendations

For a production or permanent installation, consider the following: use an external 5V power supply rather than USB, add a flyback diode across each relay coil, house the electronics in a suitable enclosure, include fuse protection on the power rail, enable OTA (over-the-air) firmware updates, and secure the MQTT broker with authentication and TLS.

---

## 15. Troubleshooting

**`LEDC is not initialized`** — Ensure `ledcSetup()` is called before any `ledcWrite()` calls in the firmware.

**Buzzer stuck ON** — Check for blocking `delay()` calls that may prevent the buzzer from being turned off. Verify that the master switch logic correctly resets the buzzer state.

**MQTT not connecting** — Confirm the IP address in `config.h` is correct, check that no firewall is blocking port 1883, and ensure the ESP32 and the broker are on the same Wi-Fi network.

---

## Final Deployment Sequence

1. Update all credentials in `include/config.h`
2. Flash firmware to the ESP32
3. Verify Wi-Fi connection
4. Verify Blynk dashboard connectivity
5. Verify MQTT data publishing
6. Test BLE commands
7. Test RFID enable/disable
8. Validate automation logic
9. Complete final enclosure assembly

---

## System Status

The system is fully integrated across the following control layers: local physical control, Blynk cloud control, BLE control, MQTT data logging, automation engine, and RFID master security switch.