
# MQTT Topic Architecture
### Smart IoT Automation System

This document defines the MQTT topic structure used in the system. The architecture is designed to be scalable, modular, multi-device ready, and straightforward to integrate with Node-RED and cloud services.

---

## 1. Base Topic Structure

All MQTT topics follow this pattern:

```
iot/<location>/<category>/<type>
```

For this project, the base path is `iot/home/...`

---

## 2. Full Topic Hierarchy

### Sensor Data (Published by ESP32)

| Topic | Direction | Description |
|---|---|---|
| `iot/home/temperature` | Publish | Temperature in °C |
| `iot/home/humidity` | Publish | Humidity in % |
| `iot/home/light` | Publish | LDR value |
| `iot/home/distance` | Publish | Distance in cm |
| `iot/home/motion` | Publish | PIR sensor status |

### System Status (Published by ESP32)

| Topic | Direction | Description |
|---|---|---|
| `iot/home/system/status` | Publish | Current system state (ON / OFF) |
| `iot/home/system/mode` | Publish | Active operating mode |
| `iot/home/system/rfid` | Publish | RFID scan events |

### Control Topics (Subscribed by ESP32)

| Topic | Direction | Description |
|---|---|---|
| `iot/home/control/fan` | Subscribe | Fan control (ON / OFF) |
| `iot/home/control/system` | Subscribe | Master system control |
| `iot/home/control/mode` | Subscribe | Mode change commands |
| `iot/home/control/buzzer` | Subscribe | Alert trigger |

### Debug

| Topic | Direction | Description |
|---|---|---|
| `iot/home/debug` | Publish | System logs and diagnostic output |

---

## 3. Data Flow

```
ESP32 → MQTT Broker → Node-RED → Dashboard
                    ↕
              Remote Commands
```

---

## 4. Payload Format

### Current Implementation

Payloads are currently simple scalar values:

```
25.3
ON
OFF
1
0
```

### Recommended Format (Future Upgrade)

A JSON-based payload structure is recommended for production use:

```json
{
  "temperature": 25.3,
  "humidity": 60,
  "light": 1780,
  "distance": 4.8,
  "motion": true,
  "system": "ON"
}
```

This approach simplifies parsing, supports database storage, and allows multiple parameters to be published in a single message.

---

## 5. Node-RED Integration

Node-RED can subscribe to the wildcard topic `iot/home/#` to automatically capture all sensor data across the hierarchy. This enables dashboard visualization, database storage, and automation flows without configuring individual subscriptions per topic.

---

## 6. Scalability

To support multiple devices, the topic structure can be extended by introducing a device identifier:

```
iot/<device_id>/<category>/<type>
```

For example:

```
iot/device001/temperature
iot/device002/temperature
```

This pattern supports multi-room and multi-building deployments and is compatible with industrial-scale systems.

---

## 7. Security Considerations

For production deployments, the following measures are recommended: MQTT broker authentication, TLS encryption for all connections, topic-level access control, and role-based broker user permissions.

---

## 8. Architectural Role of MQTT

While Blynk handles cloud-based visualization, MQTT serves as the local backbone of the system. It provides local data logging, Node-RED integration, database connectivity, multi-device expansion support, and reliable offline operation within the local network. The two layers are complementary — MQTT manages local infrastructure while Blynk surfaces data to end users remotely.

---

## 9. Summary

This topic architecture cleanly separates sensor data from control commands, supports bidirectional communication between the ESP32 and external systems, and follows a hierarchical naming convention that scales from single-room setups to multi-device deployments.

