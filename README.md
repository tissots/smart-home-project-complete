Content is user-generated and unverified.
1
Smart Home Controller – ESP32
<svg width="1200" height="400" viewBox="0 0 1200 400" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <!-- Background gradient -->
    <linearGradient id="bgGrad" x1="0%" y1="0%" x2="100%" y2="100%">
      <stop offset="0%" style="stop-color:#0a0f1e;stop-opacity:1" />
      <stop offset="50%" style="stop-color:#0d1b2a;stop-opacity:1" />
      <stop offset="100%" style="stop-color:#0a1628;stop-opacity:1" />
    </linearGradient>
    <!-- Accent glow -->
    <radialGradient id="glowLeft" cx="25%" cy="50%" r="45%">
      <stop offset="0%" style="stop-color:#00c9ff;stop-opacity:0.12" />
      <stop offset="100%" style="stop-color:#00c9ff;stop-opacity:0" />
    </radialGradient>
    <radialGradient id="glowRight" cx="80%" cy="50%" r="40%">
      <stop offset="0%" style="stop-color:#f97316;stop-opacity:0.1" />
      <stop offset="100%" style="stop-color:#f97316;stop-opacity:0" />
    </radialGradient>
    <!-- Grid pattern -->
    <pattern id="grid" width="40" height="40" patternUnits="userSpaceOnUse">
      <path d="M 40 0 L 0 0 0 40" fill="none" stroke="#1a2a3a" stroke-width="0.5"/>
    </pattern>
    <!-- Icon clip -->
    <clipPath id="iconClip">
      <circle cx="220" cy="200" r="90"/>
    </clipPath>
    <!-- Blur filter for glow -->
    <filter id="blur4">
      <feGaussianBlur stdDeviation="4"/>
    </filter>
    <filter id="blur8">
      <feGaussianBlur stdDeviation="8"/>
    </filter>
    <filter id="blur2">
      <feGaussianBlur stdDeviation="2"/>
    </filter>
  </defs>

  <!-- Base background -->
  <rect width="1200" height="400" fill="url(#bgGrad)"/>
  <!-- Grid overlay -->
  <rect width="1200" height="400" fill="url(#grid)" opacity="0.6"/>
  <!-- Glow overlays -->
  <rect width="1200" height="400" fill="url(#glowLeft)"/>
  <rect width="1200" height="400" fill="url(#glowRight)"/>

  <!-- Decorative horizontal lines -->
  <line x1="0" y1="1" x2="1200" y2="1" stroke="#00c9ff" stroke-width="1.5" opacity="0.4"/>
  <line x1="0" y1="399" x2="1200" y2="399" stroke="#00c9ff" stroke-width="1.5" opacity="0.4"/>

  <!-- Left accent bar -->
  <rect x="0" y="0" width="4" height="400" fill="#00c9ff" opacity="0.7"/>
  <!-- Right accent bar -->
  <rect x="1196" y="0" width="4" height="400" fill="#f97316" opacity="0.5"/>

  <!-- ── Circuit / Icon artwork (left side) ── -->
  <!-- Outer ring glow -->
  <circle cx="220" cy="200" r="96" fill="none" stroke="#00c9ff" stroke-width="1" opacity="0.25" filter="url(#blur4)"/>
  <!-- Ring -->
  <circle cx="220" cy="200" r="90" fill="none" stroke="#00c9ff" stroke-width="1.2" opacity="0.5"/>
  <!-- Inner ring -->
  <circle cx="220" cy="200" r="70" fill="none" stroke="#00c9ff" stroke-width="0.6" opacity="0.3"/>
  <!-- Core circle -->
  <circle cx="220" cy="200" r="52" fill="#0d2035" stroke="#00c9ff" stroke-width="1.5" opacity="0.9"/>
  <!-- ESP32 chip body -->
  <rect x="196" y="176" width="48" height="48" rx="4" fill="#0a1628" stroke="#00c9ff" stroke-width="1.2"/>
  <!-- Chip pins left -->
  <rect x="186" y="184" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="186" y="192" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="186" y="200" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="186" y="208" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="186" y="216" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <!-- Chip pins right -->
  <rect x="244" y="184" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="244" y="192" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="244" y="200" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="244" y="208" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <rect x="244" y="216" width="10" height="3" rx="1" fill="#00c9ff" opacity="0.7"/>
  <!-- Chip label -->
  <text x="220" y="196" font-family="monospace" font-size="8" fill="#00c9ff" text-anchor="middle" opacity="0.9">ESP32</text>
  <text x="220" y="208" font-family="monospace" font-size="6" fill="#4ad4f5" text-anchor="middle" opacity="0.7">WROOM</text>
  <text x="220" y="219" font-family="monospace" font-size="5.5" fill="#4ad4f5" text-anchor="middle" opacity="0.6">32-D</text>

  <!-- Circuit traces radiating from ring -->
  <line x1="310" y1="200" x2="360" y2="200" stroke="#00c9ff" stroke-width="0.8" opacity="0.3"/>
  <line x1="260" y1="127" x2="290" y2="100" stroke="#00c9ff" stroke-width="0.8" opacity="0.3"/>
  <line x1="260" y1="273" x2="290" y2="300" stroke="#00c9ff" stroke-width="0.8" opacity="0.3"/>
  <line x1="130" y1="200" x2="90" y2="200" stroke="#00c9ff" stroke-width="0.8" opacity="0.25"/>

  <!-- Small node dots on traces -->
  <circle cx="360" cy="200" r="3" fill="#00c9ff" opacity="0.4"/>
  <circle cx="290" cy="100" r="3" fill="#00c9ff" opacity="0.4"/>
  <circle cx="290" cy="300" r="3" fill="#00c9ff" opacity="0.4"/>

  <!-- Tick marks on outer ring -->
  <line x1="220" y1="108" x2="220" y2="118" stroke="#00c9ff" stroke-width="1.2" opacity="0.5"/>
  <line x1="220" y1="282" x2="220" y2="292" stroke="#00c9ff" stroke-width="1.2" opacity="0.5"/>
  <line x1="128" y1="200" x2="138" y2="200" stroke="#00c9ff" stroke-width="1.2" opacity="0.5"/>
  <line x1="302" y1="200" x2="312" y2="200" stroke="#00c9ff" stroke-width="1.2" opacity="0.5"/>

  <!-- ── Main text (right side) ── -->
  <!-- "SMART HOME" large title -->
  <text x="430" y="175" font-family="'Courier New', monospace" font-size="62" font-weight="700"
        fill="#e8f4fd" letter-spacing="4" opacity="0.95">SMART HOME</text>
  <!-- Glow duplicate -->
  <text x="430" y="175" font-family="'Courier New', monospace" font-size="62" font-weight="700"
        fill="#00c9ff" letter-spacing="4" opacity="0.08" filter="url(#blur8)">SMART HOME</text>

  <!-- "CONTROLLER" subtitle -->
  <text x="432" y="228" font-family="'Courier New', monospace" font-size="38" font-weight="400"
        fill="#4ad4f5" letter-spacing="14" opacity="0.85">CONTROLLER</text>

  <!-- Divider line -->
  <line x1="430" y1="248" x2="1080" y2="248" stroke="#00c9ff" stroke-width="0.8" opacity="0.25"/>

  <!-- Tag line -->
  <text x="432" y="272" font-family="'Courier New', monospace" font-size="15" fill="#6b8fa8"
        letter-spacing="2" opacity="0.8">ESP32  ·  FreeRTOS  ·  MQTT  ·  Blynk  ·  Node-RED  ·  Bluetooth</text>

  <!-- Corner decorations -->
  <!-- Top-left corner bracket -->
  <path d="M 20 20 L 20 60 M 20 20 L 60 20" stroke="#00c9ff" stroke-width="1.5" fill="none" opacity="0.4"/>
  <!-- Top-right corner bracket -->
  <path d="M 1180 20 L 1180 60 M 1180 20 L 1140 20" stroke="#00c9ff" stroke-width="1.5" fill="none" opacity="0.4"/>
  <!-- Bottom-left corner bracket -->
  <path d="M 20 380 L 20 340 M 20 380 L 60 380" stroke="#00c9ff" stroke-width="1.5" fill="none" opacity="0.4"/>
  <!-- Bottom-right corner bracket -->
  <path d="M 1180 380 L 1180 340 M 1180 380 L 1140 380" stroke="#f97316" stroke-width="1.5" fill="none" opacity="0.4"/>

  <!-- Small status dots top right -->
  <circle cx="1100" cy="35" r="5" fill="#22c55e" opacity="0.8"/>
  <text x="1112" y="39" font-family="monospace" font-size="10" fill="#6b8fa8" opacity="0.7">ONLINE</text>
  <circle cx="1100" cy="55" r="5" fill="#f97316" opacity="0.7"/>
  <text x="1112" y="59" font-family="monospace" font-size="10" fill="#6b8fa8" opacity="0.7">MQTT</text>
  <circle cx="1100" cy="75" r="5" fill="#00c9ff" opacity="0.7"/>
  <text x="1112" y="79" font-family="monospace" font-size="10" fill="#6b8fa8" opacity="0.7">BT</text>

  <!-- Bottom-right version tag -->
  <text x="1160" y="390" font-family="monospace" font-size="10" fill="#2a4a6a"
        text-anchor="end" opacity="0.6">v1.0</text>
</svg>



A complete smart home automation system built on the ESP32 microcontroller. The system supports manual, automatic, and remote control through physical buttons, a Blynk smartphone app, MQTT/Node-RED, and Bluetooth Classic — all running simultaneously.

<!-- Replace with a real project photo --> <!-- ![Project Photo](docs/media/images/project-photo.jpg) -->
Demo
YouTube — LINK_TO_YOUR_VIDEO
📸 Screenshots and GIFs for each feature are located in docs/media/images/

About
Most "smart home" systems just switch things on and off — this one actually thinks.

This project goes beyond basic relay control. It uses event-driven logic and sensor fusion to make contextually aware decisions:

PIR-based lighting that remembers whether the light was manually turned on before acting on motion
Temperature-driven fan automation with configurable thresholds
RFID master card that acts as a physical system kill switch
A Party Mode with RGB light patterns — because why not
Features
PIR Motion Lighting — turns off after 2 min of inactivity; restores on motion only if light was previously on; ignores motion if manually switched off
Temperature-Based Fan Control — DHT11 sensor triggers fan relay at configurable threshold
Manual Relay Control — 5 ADC push buttons for direct device switching
Bluetooth Classic — full offline control when Wi-Fi is unavailable
MQTT Integration — remote monitoring and control via any MQTT client
Blynk App — smartphone interface with virtual pin mapping
Node-RED Dashboard — flow-based automation and live visualization
Party Mode — RGB/WS2812 LED light show, triggerable via app or MQTT
RFID Master Card — toggle the entire system on/off with a physical card
Hardware
Requirements
Component	Qty	Notes
ESP32 Dev Module	1	Any standard ESP32 board
Relay Modules	4	Controls lights and fan
DHT11 Temperature Sensor	1	Fan automation
PIR Motion Sensor	1	Indoor light control
RGB LED / WS2812	1	Party Mode lighting
HC-SR04 Ultrasonic	1	Optional proximity detection
RFID Reader MFRC522	1	Master card system toggle
Push Buttons (ADC)	5	Manual relay control
Buzzer / Piezo	1	Feedback and alerts
Wires, Breadboard, etc.	—	Prototyping
Schematic
Full wiring details are in docs/hardware-wiring-diagram.md.

<!-- Uncomment and update path when diagram is ready --> <!-- ![Wiring Diagram](docs/media/images/wiring-diagram.png) -->
Repository Structure
smart-home-esp32/
├─ README.md
├─ LICENSE.md
├─ docs/
│  ├─ hardware-wiring-diagram.md
│  ├─ deployment-guide.md
│  ├─ mqtt-topic-architecture.md
│  ├─ nodered-dashboard-overview.md
│  └─ media/
│     ├─ images/          ← screenshots, wiring diagram, GIFs
│     └─ videos/          ← demo video
├─ firmware/
│  ├─ src/
│  │  └─ main.ino
│  ├─ include/
│  │  ├─ config.h         ← WiFi, Blynk credentials
│  │  └─ topics.h         ← MQTT topic definitions
│  └─ lib/
├─ blynk-config/
│  └─ template.json
├─ nodered/
│  └─ flow.json
└─ .gitignore
Setup & Deployment
1. Firmware
Clone the repo and navigate to the firmware folder:

bash
git clone https://github.com/YOUR_USERNAME/smart-home-esp32.git
cd smart-home-esp32/firmware
Open main.ino (or the src/ directory) in PlatformIO or Arduino IDE.

Update config.h with your credentials:

cpp
#define WIFI_SSID        "YOUR_WIFI_SSID"
#define WIFI_PASSWORD    "YOUR_WIFI_PASSWORD"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"
Update topics.h with your MQTT broker address and topic names, then compile and flash to the ESP32.

2. Blynk App
Import the template from blynk-config/template.json
Connect using the same Blynk auth token from config.h
Map virtual pins as described in docs/deployment-guide.md
3. Node-RED Dashboard
Import the flow from nodered/flow.json
Review layout details in docs/nodered-dashboard-overview.md
Update MQTT broker settings to match topics.h
MQTT Architecture
Full topic documentation is in docs/mqtt-topic-architecture.md.

Topic	Direction	Purpose
smarthome/esp32/set	Inbound	Commands from Node-RED / Blynk
smarthome/esp32/status	Outbound	System state updates
smarthome/esp32/event	Outbound	Events — motion, RFID scan, party mode, etc.
Control Modes
The system supports four independent control modes that can operate simultaneously:

Manual — physical buttons or Blynk virtual pins
Automatic — onboard sensors (PIR, DHT11) trigger actions autonomously
Bluetooth — offline control via Bluetooth Classic when Wi-Fi is unavailable
Remote — MQTT commands and Node-RED flows for advanced automation
Party Mode is triggered via Blynk virtual pin V33 or the MQTT command PARTY ON.

License
This project is licensed under the MIT License.

Creator
[Your Name Here]

Show Image
Show Image
Show Image

