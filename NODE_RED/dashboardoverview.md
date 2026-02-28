# Node-RED Dashboard Overview
### Smart IoT Automation System

This document describes the dashboard layout, widget configuration, and flow logic for monitoring and controlling the ESP32-based smart home system.

---

## 1. Dashboard Purpose

The dashboard serves four primary functions: real-time sensor monitoring (temperature, humidity, light, motion, and distance), system control (fans, lights, buzzers, and modes), automation visualization (current mode and active triggers), and debug logging (events, anomalies, and alerts).

---

## 2. Layout Design

The dashboard is organized into four tabs for clarity:

| Tab | Purpose | Widgets |
|---|---|---|
| Home | Real-time sensor monitoring | Gauges (Temp/Humidity), LED indicators (Motion), Charts (Light, Distance) |
| Control | System control | Switches (Fan, Buzzer), Dropdown (Mode), Buttons (System ON/OFF) |
| Logs | Debug and alerts | Text area or Table node displaying event logs |
| Settings | Configuration overview | Node-RED settings info, MQTT connection status |

---

## 3. Node & Widget Mapping

### Sensors

| Sensor | Node-RED Node | Widget | Tab |
|---|---|---|---|
| Temperature | MQTT In | Gauge | Home |
| Humidity | MQTT In | Gauge | Home |
| Light | MQTT In | Chart | Home |
| Motion | MQTT In | LED Indicator | Home |
| Distance | MQTT In | Chart | Home |

### Controls

| Function | Node-RED Node | Widget | Tab |
|---|---|---|---|
| Fan | MQTT Out | Switch | Control |
| Buzzer | MQTT Out | Switch | Control |
| System Mode | MQTT Out | Dropdown | Control |
| Master ON/OFF | MQTT Out | Button | Control |

### Logs

The Logs tab uses a Text or Table node to display sensor anomalies, system events, and errors. It subscribes to the `iot/home/debug` topic.

---

## 4. Layout Guidelines

Group related widgets together — for example, all environmental sensors in one row and controls in another. Use consistent color coding for status indicators: green for OK, yellow for warning, and red for alert or fault. Charts should update in real time using a sliding window of 30–60 seconds.

---

## 5. Flow Logic

MQTT In nodes receive sensor data and update the corresponding dashboard widgets. UI widgets such as buttons and switches trigger MQTT Out nodes, which send commands back to the ESP32. Optional Function nodes handle automation rules — for example, turning on the light when motion is detected, or activating the fan when temperature exceeds a defined threshold. A Debug node captures all MQTT traffic for troubleshooting.

---

## 6. Scalability

The tab and widget structure can scale across multiple devices by namespacing MQTT topics per device (e.g., `iot/device001/...`, `iot/device002/...`). Template nodes should be used where possible to avoid duplicating flow logic.

---

## 7. Recommended Enhancements

Consider the following improvements as the system matures: adding charts that retain the last 5–10 minutes of sensor readings, popup notifications for critical events, a responsive mobile layout, and an optional dark mode for long-term monitoring environments.

---

## 8. Integration with the ESP32

The dashboard reflects the real-time state of the ESP32 via MQTT. All user actions are forwarded to the ESP32 through the designated control topics. The system operates fully offline provided the MQTT broker is hosted locally.

---

## 9. Summary
