// =============================================================================
//  SMART HOME CONTROLLER – ESP32  
//  PIR logic:
//  - Turns light OFF after 2 min no motion (if ON)
//  - Motion restores light ON only if previously desired ON (manual or PIR)
//  - Motion while ON resets 2-min timer
//  - Manual OFF → motion does nothing (stays OFF)
// =============================================================================

#define BLYNK_TEMPLATE_ID   ""
#define BLYNK_TEMPLATE_NAME "esp 32 relay control"
#define BLYNK_AUTH_TOKEN    ""

const char* mqtt_server = "";
const int mqtt_port = 1883;
const char* mqtt_user = "";
const char* mqtt_password = "";
const char* mqtt_base_topic = "smarthome/esp32";

#include <Arduino.h>
#include "DHT.h"
#include <SPI.h>
#include <MFRC522.h>
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <BluetoothSerial.h>
#include <PubSubClient.h>

BluetoothSerial SerialBT;
BlynkTimer timer;
WiFiClient espClient;
PubSubClient client(espClient);

char ssid[] = "";
char pass[] = "";

#define LDR_PIN         33
#define BUTTON_ADC_PIN  34
#define RELAY1_PIN      21
#define RELAY2_PIN      25
#define RELAY3_PIN      14
#define RELAY4_PIN      26
#define PIR_PIN         5
#define DHT_PIN         4
#define RED_PIN         13
#define GREEN_PIN       17
#define BLUE_PIN        27
#define BUZZER_PIN      22
#define RFID_SS_PIN     12
#define RFID_RST_PIN    16
#define TRIG_PIN        32
#define ECHO_PIN        35

const int   MY_RGB_BRIGHTNESS   = 80;
const int   LDR_LOW_THRESHOLD   = 450;
const int   LDR_HIGH_THRESHOLD  = 550;
const float DHT_TEMP_OFFSET     = -5.0f;
const float FAN_THRESHOLD       = 25.0f;
const float TEMP_MIN_VALID      = -40.0f;
const float TEMP_MAX_VALID      = 80.0f;

const unsigned long PIR_AUTO_OFF           = 120000UL;  // 2 minutes
const unsigned long PIR_CONFIRM_TIME       = 1500UL;
const unsigned long PIR_RELEASE_TIME       = 3000UL;
const unsigned long BUTTON_COOLDOWN        = 300UL;
const unsigned long SERIAL_INTERVAL        = 1500UL;
const unsigned long ULTRA_READ_INTERVAL    = 200UL;
const unsigned long WDT_TIMEOUT            = 10000UL;
const int   ULTRA_THRESHOLD_NEAR           = 10;
const int   ULTRA_THRESHOLD_FAR            = 12;
const unsigned long ULTRA_TIMEOUT_MS       = 5000UL;
const unsigned long PROXIMITY_STABLE_TIME  = 400UL;

const int BTN1_MIN=200, BTN1_MAX=400, BTN2_MIN=500, BTN2_MAX=600;
const int BTN3_MIN=1100, BTN3_MAX=1200, BTN4_MIN=1900, BTN4_MAX=2000;
const int BTN5_MIN=2700, BTN5_MAX=2800;

const unsigned long PARTY_UPDATE_INTERVAL  = 20UL;
const unsigned long PARTY_CHANGE_INTERVAL  = 4000UL;

struct Relay { bool state; bool manualMode; };
enum FanMode { MANUAL_OFF, MANUAL_ON, AUTO };

Relay relay1 = {false, true};
Relay relay2 = {false, false};
Relay relay3 = {false, false};
Relay relay4 = {false, true};

FanMode fanMode = AUTO;

DHT dht(DHT_PIN, DHT11);
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
Preferences prefs;

const byte masterCard[4] = {0xE9, 0xA3, 0x4D, 0x06};

float temperature = 0.0f;
bool systemActive = false;
bool manualLock   = false;
bool partyMode    = false;

unsigned long lastPressTime  = 0;

unsigned long pirLastMotion      = 0;
bool          pirConfirmed       = false;
bool          lastPirConfirmed   = false;
unsigned long pirHighSince       = 0;
unsigned long pirLowSince        = 0;
bool          pirDesiredOn       = false;  // remembers if last desired state was ON

bool buzzerActive = false;

unsigned long lastSerialPrint   = 0;
unsigned long lastUltraRead     = 0;
unsigned long lastValidUltraReading = 0;
long ultraDistance              = -1;
bool isClose                    = false;

unsigned long momentaryGreenEndTime = 0;
unsigned long purpleEndTime         = 0;
unsigned long proximityStableSince  = 0;

unsigned long lastBlinkToggle   = 0;
bool blinkState                 = false;

int lastLdrValue = 0;

unsigned long tempHighSince       = 0;
unsigned long tempLowSince        = 0;
bool          lastAppliedFanState = false;

unsigned long ldrDarkSince        = 0;
unsigned long ldrBrightSince      = 0;
bool          lastAppliedLdrState = false;

unsigned long lastPartyUpdate   = 0;
unsigned long lastPartyChange   = 0;
uint8_t       partyPhase        = 0;
uint8_t       partyPattern      = 0;
uint8_t       partySpeed        = 2;
uint8_t       partyBeat         = 0;
uint8_t       partyStrobeCount  = 0;

unsigned long lastBtStatus      = 0;
const unsigned long BT_STATUS_INTERVAL = 4000UL;

unsigned long lastMqttStatus    = 0;
const unsigned long MQTT_STATUS_INTERVAL = 10000UL;

unsigned long lastValidTempRead = 0;

void handleRFID();
void handleButtons(unsigned long now);
void updateSensors(unsigned long now);
void handlePIR(unsigned long now);
void updateUltrasonic(unsigned long now);
void updateBuzzer();
void applyRelays();
void updateRGB(unsigned long now);
void updatePartyMode(unsigned long now);
void printStatus(unsigned long now);
void saveSystemState();
void loadSystemState();
int readButton();
long readUltraDistanceCM();
bool checkMasterCard();
void setRGB(bool wantRed, bool wantGreen, bool wantBlue);
void turnRGBOff();
void forceRelaysOff();
const char* getFanModeString(FanMode mode);
void sendToBlynk();
void TaskBlynk(void *pvParameters);
void handleBluetoothCommands();
void resetAutoStates();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnectMqtt();
void publishStatus();
void publishEvent(const char* event);

class ToneManager {
private:
    unsigned long endTime = 0;
    bool isPlaying = false;
public:
    void playTone(int frequency, unsigned long duration) {
        tone(BUZZER_PIN, frequency);
        endTime = millis() + duration;
        isPlaying = true;
    }
    void update() {
        if (isPlaying && millis() >= endTime) {
            noTone(BUZZER_PIN);
            isPlaying = false;
            endTime = 0;
        }
    }
    bool isActive() { return isPlaying; }
    void stop() {
        if (isPlaying) {
            noTone(BUZZER_PIN);
            isPlaying = false;
            endTime = 0;
        }
    }
} toneManager;

void setup() {
    Serial.begin(115200);
    delay(150);

    esp_task_wdt_init(WDT_TIMEOUT / 1000, true);
    esp_task_wdt_add(NULL);

    pinMode(RELAY1_PIN, OUTPUT); pinMode(RELAY2_PIN, OUTPUT);
    pinMode(RELAY3_PIN, OUTPUT); pinMode(RELAY4_PIN, OUTPUT);
    pinMode(PIR_PIN,    INPUT);
    pinMode(RED_PIN,    OUTPUT); pinMode(GREEN_PIN, OUTPUT); pinMode(BLUE_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(TRIG_PIN,   OUTPUT); pinMode(ECHO_PIN,  INPUT);

    digitalWrite(RELAY1_PIN, HIGH); digitalWrite(RELAY2_PIN, HIGH);
    digitalWrite(RELAY3_PIN, HIGH); digitalWrite(RELAY4_PIN, HIGH);
    noTone(BUZZER_PIN);

    turnRGBOff();

    dht.begin();
    SPI.begin();
    rfid.PCD_Init();

    prefs.begin("smarthome", false);
    loadSystemState();

    Serial.print("Restored system state: ");
    Serial.println(systemActive ? "ACTIVE" : "OFF");

    WiFi.begin(ssid, pass);
    Blynk.config(BLYNK_AUTH_TOKEN);

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqttCallback);

    xTaskCreatePinnedToCore(TaskBlynk, "BlynkTask", 4096, NULL, 1, NULL, 1);

    timer.setInterval(10000L, sendToBlynk);

    SerialBT.begin("SmartHomeESP32");
    Serial.println("Bluetooth Classic started → SmartHomeESP32");

    Serial.println("\n=== Smart Home Controller Started ===");
}

void loop() {
    unsigned long now = millis();

    esp_task_wdt_reset();

    timer.run();

    if (!client.connected()) {
        reconnectMqtt();
    }
    client.loop();

    handleRFID();
    updateUltrasonic(now);
    updateBuzzer();
    toneManager.update();

    if (!systemActive) {
        forceRelaysOff();
        partyMode = false;
    } else {
        handleButtons(now);
        updateSensors(now);
        applyRelays();
    }

    updateRGB(now);
    printStatus(now);
    handleBluetoothCommands();

    if (now - lastMqttStatus >= MQTT_STATUS_INTERVAL) {
        lastMqttStatus = now;
        publishStatus();
    }

    delay(50);
}

void reconnectMqtt() {
    static unsigned long lastAttempt = 0;
    if (millis() - lastAttempt < 5000) return;
    lastAttempt = millis();

    Serial.print("Attempting MQTT connection... ");

    String clientId = "ESP32SmartHome-" + String(random(0xffff), HEX);

    bool connected = false;
    if (strlen(mqtt_user) > 0 && strlen(mqtt_password) > 0) {
        connected = client.connect(clientId.c_str(), mqtt_user, mqtt_password);
    } else {
        connected = client.connect(clientId.c_str());
    }

    if (connected) {
        Serial.println("connected");

        String cmdTopic = String(mqtt_base_topic) + "/set";
        client.subscribe(cmdTopic.c_str());

        publishEvent("connected");
        publishStatus();
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
    }
}

void mqttCallback(char* topic_in, byte* payload, unsigned int length) {
    String topic = topic_in;
    String message = "";

    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    message.trim();
    String cmd = message;
    cmd.toUpperCase();

    Serial.print("MQTT RX → "); Serial.println(cmd);

    bool known = true;
    bool stateChanged = false;

    if (cmd == "ON" || cmd == "SYS ON") {
        if (!systemActive) { systemActive = true; stateChanged = true; }
    }
    else if (cmd == "OFF" || cmd == "SYS OFF") {
        if (systemActive) { systemActive = false; stateChanged = true; }
    }
    else if (cmd == "R1 ON") {
        if (systemActive && !relay1.state) { relay1.state = true; stateChanged = true; }
    }
    else if (cmd == "R1 OFF") {
        if (relay1.state) { relay1.state = false; stateChanged = true; }
    }
    else if (cmd == "R4 ON") {
        if (systemActive && !relay4.state) { relay4.state = true; pirDesiredOn = true; stateChanged = true; }
    }
    else if (cmd == "R4 OFF") {
        if (relay4.state) { relay4.state = false; pirDesiredOn = false; stateChanged = true; }
    }
    else if (cmd == "F AUTO") {
        if (fanMode != AUTO) { fanMode = AUTO; relay2.manualMode = false; stateChanged = true; resetAutoStates(); }
    }
    else if (cmd == "F ON") {
        if (fanMode != MANUAL_ON) { fanMode = MANUAL_ON; relay2.manualMode = true; stateChanged = true; }
    }
    else if (cmd == "F OFF") {
        if (fanMode != MANUAL_OFF) { fanMode = MANUAL_OFF; relay2.manualMode = true; stateChanged = true; }
    }
    else if (cmd == "R3 AUTO") {
        if (relay3.manualMode) { relay3.manualMode = false; stateChanged = true; resetAutoStates(); }
    }
    else if (cmd == "R3 ON") {
        if (!relay3.manualMode || !relay3.state) {
            relay3.manualMode = true; relay3.state = true; stateChanged = true;
        }
    }
    else if (cmd == "R3 OFF") {
        if (!relay3.manualMode || relay3.state) {
            relay3.manualMode = true; relay3.state = false; stateChanged = true;
        }
    }
    else if (cmd == "PARTY ON" || cmd == "P ON") {
        if (systemActive && !partyMode) { partyMode = true; stateChanged = true; }
    }
    else if (cmd == "PARTY OFF" || cmd == "P OFF") {
        if (partyMode) { partyMode = false; stateChanged = true; }
    }
    else {
        known = false;
        client.publish((String(mqtt_base_topic) + "/error").c_str(), "Unknown command");
    }

    if (known) {
        toneManager.playTone(1800, 120);
        if (stateChanged) {
            saveSystemState();
            applyRelays();
            publishStatus();
        }
    }
}

void publishStatus() {
    if (!client.connected()) return;

    char payload[256];
    snprintf(payload, sizeof(payload),
        "{\"system\":%d,\"temp\":%.1f,\"ldr\":%d,\"distance\":%ld,\"pir\":%d,"
        "\"r1\":%d,\"r2\":%d,\"r3\":%d,\"r4\":%d,\"party\":%d,\"fan\":\"%s\"}",
        systemActive, temperature, lastLdrValue, ultraDistance,
        pirConfirmed, relay1.state, relay2.state, relay3.state, relay4.state,
        partyMode, getFanModeString(fanMode)
    );

    String topic = String(mqtt_base_topic) + "/status";
    client.publish(topic.c_str(), payload);
}

void publishEvent(const char* msg) {
    if (!client.connected()) return;
    String topic = String(mqtt_base_topic) + "/event";
    client.publish(topic.c_str(), msg);
}

void handleBluetoothCommands() {
    unsigned long now = millis();

    if (now - lastBtStatus >= BT_STATUS_INTERVAL) {
        lastBtStatus = now;
        if (SerialBT.hasClient()) {
            SerialBT.printf("SYS:%d TEMP:%.1f LDR:%d U:%ld PIR:%d R1:%d R2:%d R3:%d R4:%d PARTY:%d\n",
                systemActive ? 1 : 0,
                temperature,
                lastLdrValue,
                ultraDistance,
                pirConfirmed ? 1 : 0,
                relay1.state ? 1 : 0,
                relay2.state ? 1 : 0,
                relay3.state ? 1 : 0,
                relay4.state ? 1 : 0,
                partyMode ? 1 : 0
            );
        }
    }

    static String btBuffer = "";
    while (SerialBT.available()) {
        char c = SerialBT.read();
        if (c == '\n' || c == '\r') {
            if (btBuffer.length() > 0) {
                String cmd = btBuffer;
                cmd.trim();
                cmd.toUpperCase();

                Serial.print("BT RX: "); Serial.println(cmd);

                bool known = true;
                bool changed = false;

                if (cmd == "ON" || cmd == "SYS ON") {
                    systemActive = true;
                    changed = true;
                    SerialBT.println("System ON");
                }
                else if (cmd == "OFF" || cmd == "SYS OFF") {
                    systemActive = false;
                    forceRelaysOff();
                    partyMode = false;
                    changed = true;
                    SerialBT.println("System OFF");
                }
                else if (cmd == "R1 ON") {
                    if (!systemActive) { SerialBT.println("ERR: System OFF"); }
                    else { relay1.state = true; changed = true; applyRelays(); saveSystemState(); SerialBT.println("R1 ON"); }
                }
                else if (cmd == "R1 OFF") { relay1.state = false; changed = true; applyRelays(); saveSystemState(); SerialBT.println("R1 OFF"); }
                else if (cmd == "R4 ON") {
                    if (!systemActive) { SerialBT.println("ERR: System OFF"); }
                    else { relay4.state = true; pirDesiredOn = true; changed = true; applyRelays(); saveSystemState(); SerialBT.println("R4 ON"); }
                }
                else if (cmd == "R4 OFF") { relay4.state = false; pirDesiredOn = false; changed = true; applyRelays(); saveSystemState(); SerialBT.println("R4 OFF"); }
                else if (cmd == "F AUTO") { fanMode = AUTO; relay2.manualMode = false; changed = true; resetAutoStates(); saveSystemState(); SerialBT.println("Fan AUTO"); }
                else if (cmd == "F ON")   { fanMode = MANUAL_ON; relay2.manualMode = true;  changed = true; saveSystemState(); SerialBT.println("Fan MANUAL ON"); }
                else if (cmd == "F OFF")  { fanMode = MANUAL_OFF; relay2.manualMode = true;  changed = true; saveSystemState(); SerialBT.println("Fan MANUAL OFF"); }
                else if (cmd == "R3 AUTO") { relay3.manualMode = false; changed = true; resetAutoStates(); saveSystemState(); SerialBT.println("R3 AUTO (LDR)"); }
                else if (cmd == "R3 ON")   { relay3.manualMode = true; relay3.state = true;  changed = true; applyRelays(); saveSystemState(); SerialBT.println("R3 MANUAL ON"); }
                else if (cmd == "R3 OFF")  { relay3.manualMode = true; relay3.state = false; changed = true; applyRelays(); saveSystemState(); SerialBT.println("R3 MANUAL OFF"); }
                else if (cmd == "PARTY ON" || cmd == "P ON") {
                    if (systemActive) { partyMode = true; changed = true; SerialBT.println("PARTY MODE ON"); }
                    else SerialBT.println("ERR: System OFF");
                }
                else if (cmd == "PARTY OFF" || cmd == "P OFF") {
                    partyMode = false;
                    changed = true;
                    SerialBT.println("PARTY MODE OFF");
                }
                else if (cmd == "STATUS" || cmd == "?") {
                    lastBtStatus = 0;
                }
                else {
                    known = false;
                    SerialBT.println("Try: ON OFF R1 ON R1 OFF R4 ON R4 OFF F AUTO F ON F OFF R3 AUTO R3 ON R3 OFF PARTY ON PARTY OFF STATUS");
                }

                if (known) toneManager.playTone(1800, 120);

                if (changed) {
                    saveSystemState();
                    applyRelays();
                    publishStatus();
                }

                btBuffer = "";
            }
        } else if (c >= 32 && c <= 126) {
            if (btBuffer.length() < 32) btBuffer += c;
        }
    }
}

void handlePIR(unsigned long now) {
    bool rawPir = digitalRead(PIR_PIN);

    if (rawPir) {
        pirLowSince = 0;
        if (!pirConfirmed) {
            if (pirHighSince == 0) pirHighSince = now;
            if (now - pirHighSince >= PIR_CONFIRM_TIME) {
                pirConfirmed = true;
                pirHighSince = 0;
            }
        }
    } else {
        pirHighSince = 0;
        if (pirConfirmed) {
            if (pirLowSince == 0) pirLowSince = now;
            if (now - pirLowSince >= PIR_RELEASE_TIME) {
                pirConfirmed = false;
                pirLowSince = 0;
            }
        }
    }

    if (pirConfirmed) {
        if (pirDesiredOn && !relay4.state) {
            relay4.state = true;
            Serial.println("PIR: motion detected → restoring indoor light ON (previously desired ON)");
            if (SerialBT.hasClient()) SerialBT.println("Indoor light ON (motion - restored)");
            prefs.putBool("r4_state", relay4.state);
            publishStatus();
        }

        if (relay4.state) {
            pirLastMotion = now;
        }

        if (!lastPirConfirmed && SerialBT.hasClient()) {
            SerialBT.println("Motion detected!");
            toneManager.playTone(1200, 150);
        }
    }

    if (!pirConfirmed && pirLastMotion > 0 && (now - pirLastMotion >= PIR_AUTO_OFF)) {
        if (relay4.state) {
            relay4.state = false;
            Serial.println("PIR: 2 min no motion → indoor light OFF");
            if (SerialBT.hasClient()) SerialBT.println("Indoor light OFF (2 min no motion)");
            prefs.putBool("r4_state", relay4.state);
            publishStatus();
        }
        pirLastMotion = 0;
    }

    lastPirConfirmed = pirConfirmed;
}

void TaskBlynk(void *pvParameters) {
    for (;;) {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.reconnect();
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }
        Blynk.run();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void sendToBlynk() {
    Blynk.virtualWrite(V1,  temperature);
    Blynk.virtualWrite(V2,  lastLdrValue);
    Blynk.virtualWrite(V3,  ultraDistance);
    Blynk.virtualWrite(V4,  pirConfirmed ? 1 : 0);
    Blynk.virtualWrite(V5,  relay2.state);
    Blynk.virtualWrite(V6,  relay3.state);
    Blynk.virtualWrite(V8,  systemActive ? 1 : 0);
    Blynk.virtualWrite(V9,  isClose ? 1 : 0);
    Blynk.virtualWrite(V33, partyMode ? 1 : 0);
}

BLYNK_WRITE(V10) {
    systemActive = param.asInt();
    saveSystemState();
    if (!systemActive) {
        forceRelaysOff();
        partyMode = false;
        Blynk.virtualWrite(V33, 0);
    }
    publishStatus();
}

BLYNK_WRITE(V11) { relay1.state = param.asInt(); saveSystemState(); applyRelays(); publishStatus(); }

BLYNK_WRITE(V12) {
    FanMode oldMode = fanMode;
    fanMode = (FanMode)param.asInt();
    relay2.manualMode = (fanMode != AUTO);
    if (fanMode == AUTO && oldMode != AUTO) resetAutoStates();
    saveSystemState();
    publishStatus();
}

BLYNK_WRITE(V13) { 
    bool oldManual = relay3.manualMode;
    relay3.manualMode = param.asInt(); 
    if (!relay3.manualMode && oldManual) resetAutoStates();
    saveSystemState(); 
    publishStatus(); 
}

BLYNK_WRITE(V14) { 
    relay4.state = param.asInt(); 
    pirDesiredOn = relay4.state;
    saveSystemState(); 
    applyRelays(); 
    publishStatus(); 
}

BLYNK_WRITE(V30) {
    manualLock = (param.asInt() == 1);
    Serial.println(manualLock ? "Buttons LOCKED" : "Buttons UNLOCKED");
}

BLYNK_WRITE(V33) {
    if (!systemActive) {
        Blynk.virtualWrite(V33, 0);
        Serial.println("V33: System OFF – party mode blocked");
        return;
    }
    partyMode = (param.asInt() == 1);
    Serial.println(partyMode ? "V33: PARTY MODE ON" : "V33: Party mode OFF");
    toneManager.playTone(partyMode ? 1800 : 1000, 300);
    publishStatus();
}

BLYNK_WRITE(V34) {
    if (param.asInt() == 1) {
        Serial.println("Alarm triggered – 5s buzzer");
        toneManager.playTone(2200, 5000);
        Blynk.virtualWrite(V34, 0);
    }
}

int readButton() {
    int val = analogRead(BUTTON_ADC_PIN);
    if (val >= BTN1_MIN && val <= BTN1_MAX) return 1;
    if (val >= BTN2_MIN && val <= BTN2_MAX) return 2;
    if (val >= BTN3_MIN && val <= BTN3_MAX) return 3;
    if (val >= BTN4_MIN && val <= BTN4_MAX) return 4;
    if (val >= BTN5_MIN && val <= BTN5_MAX) return 5;
    return 0;
}

bool checkMasterCard() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial() || rfid.uid.size != 4)
        return false;
    for (byte i = 0; i < 4; i++)
        if (rfid.uid.uidByte[i] != masterCard[i]) return false;
    rfid.PICC_HaltA();
    return true;
}

void handleRFID() {
    if (!checkMasterCard()) return;

    toneManager.playTone(2000, 300);

    bool wasActive = systemActive;
    systemActive   = !systemActive;

    saveSystemState();

    Serial.print("RFID Master → System ");
    Serial.println(systemActive ? "ACTIVE" : "OFF");

    if (systemActive && !wasActive) {
        relay4.state = true;
        pirDesiredOn = true;
        momentaryGreenEndTime = millis() + 2000UL;
    }

    if (!systemActive && wasActive) {
        forceRelaysOff();
        partyMode     = false;
        purpleEndTime = millis() + 1500UL;
    }

    publishStatus();
}

long readUltraDistanceCM() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
    if (duration == 0) return -1;
    return duration * 0.034 / 2;
}

void updateUltrasonic(unsigned long now) {
    if (now - lastUltraRead >= ULTRA_READ_INTERVAL) {
        lastUltraRead = now;
        long distance = readUltraDistanceCM();

        if (distance > 0) {
            lastValidUltraReading = now;
            ultraDistance         = distance;

            bool nowClose = isClose
                ? (distance <= ULTRA_THRESHOLD_FAR)
                : (distance <= ULTRA_THRESHOLD_NEAR);

            if (nowClose != isClose) {
                if (proximityStableSince == 0) proximityStableSince = now;
                if (now - proximityStableSince >= PROXIMITY_STABLE_TIME) {
                    isClose               = nowClose;
                    proximityStableSince  = 0;
                }
            } else {
                proximityStableSince = 0;
            }
        }
    }

    if (now - lastValidUltraReading > ULTRA_TIMEOUT_MS) {
        if (isClose || buzzerActive) {
            isClose = false;
            if (buzzerActive) {
                noTone(BUZZER_PIN);
                buzzerActive = false;
            }
        }
        proximityStableSince = 0;
    }
}

void updateBuzzer() {
    if (isClose) {
        if (!buzzerActive) {
            tone(BUZZER_PIN, 2200);
            buzzerActive = true;
        }
    } else if (buzzerActive) {
        noTone(BUZZER_PIN);
        buzzerActive = false;
    }
}

void applyRelays() {
    digitalWrite(RELAY1_PIN, relay1.state ? LOW : HIGH);
    digitalWrite(RELAY2_PIN, relay2.state ? LOW : HIGH);
    digitalWrite(RELAY3_PIN, relay3.state ? LOW : HIGH);
    digitalWrite(RELAY4_PIN, relay4.state ? LOW : HIGH);
}

void forceRelaysOff() {
    digitalWrite(RELAY1_PIN, HIGH);
    digitalWrite(RELAY2_PIN, HIGH);
    digitalWrite(RELAY3_PIN, HIGH);
    digitalWrite(RELAY4_PIN, HIGH);
}

void setRGB(bool wantRed, bool wantGreen, bool wantBlue) {
    analogWrite(GREEN_PIN, wantRed   ? 255 - MY_RGB_BRIGHTNESS : 255);
    analogWrite(BLUE_PIN,  wantGreen ? 255 - MY_RGB_BRIGHTNESS : 255);
    analogWrite(RED_PIN,   wantBlue  ? 255 - MY_RGB_BRIGHTNESS : 255);
}

void turnRGBOff() {
    analogWrite(RED_PIN,   255);
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN,  255);
}

void saveSystemState() {
    prefs.putBool("active",    systemActive);
    prefs.putBool("r1_state",  relay1.state);
    prefs.putBool("r1_manual", relay1.manualMode);
    prefs.putBool("r3_manual", relay3.manualMode);
    prefs.putBool("r4_state",  relay4.state);
    prefs.putUChar("fan_mode", (uint8_t)fanMode);
    prefs.putBool("pir_desired_on", pirDesiredOn);
}

void loadSystemState() {
    systemActive      = prefs.getBool("active",    false);
    relay1.state      = prefs.getBool("r1_state",  false);
    relay1.manualMode = prefs.getBool("r1_manual", true);
    relay3.manualMode = prefs.getBool("r3_manual", false);
    relay4.state      = prefs.getBool("r4_state",  false);
    fanMode           = (FanMode)prefs.getUChar("fan_mode", AUTO);
    pirDesiredOn      = prefs.getBool("pir_desired_on", false);
}

const char* getFanModeString(FanMode mode) {
    switch (mode) {
        case MANUAL_OFF: return "MANUAL_OFF";
        case MANUAL_ON:  return "MANUAL_ON";
        case AUTO:       return "AUTO";
        default:         return "UNKNOWN";
    }
}

void resetAutoStates() {
    lastAppliedFanState = relay2.state;
    tempHighSince = tempLowSince = 0;

    lastAppliedLdrState = relay3.state;
    ldrDarkSince = ldrBrightSince = 0;
}

void updateSensors(unsigned long now) {
    handlePIR(now);

    int ldrValue = analogRead(LDR_PIN);
    lastLdrValue = ldrValue;

    bool isDark   = (ldrValue < LDR_LOW_THRESHOLD);
    bool isBright = (ldrValue > LDR_HIGH_THRESHOLD);

    if (!relay3.manualMode) {
        if (isDark || isBright) {
            bool targetOn = isDark;

            if (targetOn) {
                if (ldrDarkSince == 0) ldrDarkSince = now;
                ldrBrightSince = 0;
                if (now - ldrDarkSince >= 10000UL && !lastAppliedLdrState) {
                    relay3.state = true;
                    lastAppliedLdrState = true;
                    Serial.print("LDR DARK → Relay3 ON (value="); Serial.println(ldrValue);
                }
            } else {
                if (ldrBrightSince == 0) ldrBrightSince = now;
                ldrDarkSince = 0;
                if (now - ldrBrightSince >= 10000UL && lastAppliedLdrState) {
                    relay3.state = false;
                    lastAppliedLdrState = false;
                    Serial.print("LDR BRIGHT → Relay3 OFF (value="); Serial.println(ldrValue);
                }
            }
        } else {
            ldrDarkSince = ldrBrightSince = 0;
        }
    }

    float t = dht.readTemperature() + DHT_TEMP_OFFSET;
    if (!isnan(t) && t >= TEMP_MIN_VALID && t <= TEMP_MAX_VALID) {
        temperature = t;
        lastValidTempRead = now;
    }

    if (fanMode == AUTO) {
        bool shouldFanOn = (temperature >= FAN_THRESHOLD);

        if (shouldFanOn && !lastAppliedFanState) {
            if (tempHighSince == 0) tempHighSince = now;
            tempLowSince = 0;
            if (now - tempHighSince >= 10000UL) {
                relay2.state = true;
                lastAppliedFanState = true;
                tempHighSince = 0;
                Serial.print("Temp HIGH → Fan ON ("); Serial.print(temperature); Serial.println(" °C)");
            }
        } else if (!shouldFanOn && lastAppliedFanState) {
            if (tempLowSince == 0) tempLowSince = now;
            tempHighSince = 0;
            if (now - tempLowSince >= 10000UL) {
                relay2.state = false;
                lastAppliedFanState = false;
                tempLowSince = 0;
                Serial.print("Temp LOW → Fan OFF ("); Serial.print(temperature); Serial.println(" °C)");
            }
        } else {
            tempHighSince = tempLowSince = 0;
        }

        if (now - lastValidTempRead > 30000UL && relay2.state) {
            relay2.state = false;
            lastAppliedFanState = false;
            tempHighSince = tempLowSince = 0;
            Serial.println("No valid DHT for 30s → Fan forced OFF");
        }
    } else {
        relay2.state = (fanMode == MANUAL_ON);
        tempHighSince = tempLowSince = 0;
    }
}

void printStatus(unsigned long now) {
    if (now - lastSerialPrint < SERIAL_INTERVAL) return;
    lastSerialPrint = now;

    Serial.println("─────────────────────────────────────");
    Serial.print("System: "); Serial.println(systemActive ? "ACTIVE" : "OFF");
    Serial.print("Temp:   "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("LDR:    "); Serial.println(lastLdrValue);
    Serial.print("Ultra:  "); Serial.print(ultraDistance); Serial.println(" cm");
    Serial.print("PIR:    "); Serial.println(pirConfirmed ? "MOTION" : "clear");
    Serial.print("R1:"); Serial.print(relay1.state);
    Serial.print(" R2:"); Serial.print(relay2.state);
    Serial.print(" R3:"); Serial.print(relay3.state);
    Serial.print(" R4:"); Serial.println(relay4.state);
    Serial.print("Party:  "); Serial.println(partyMode ? "ON" : "OFF");
    Serial.println("─────────────────────────────────────");
}

void updatePartyMode(unsigned long now) {
    if (!partyMode) return;

    if (now - lastPartyUpdate < PARTY_UPDATE_INTERVAL) return;
    lastPartyUpdate = now;

    if (now - lastPartyChange >= PARTY_CHANGE_INTERVAL) {
        lastPartyChange = now;
        partyPattern = (partyPattern + 1) % 6;
        partySpeed   = random(1, 4);
        Serial.print("Party pattern → ");
        Serial.println(partyPattern);
    }

    uint8_t inc = partySpeed * 5 + 3;
    partyPhase += inc;
    partyBeat  += inc * 2;

    uint8_t r = 0, g = 0, b = 0;

    switch (partyPattern) {
        case 0: {
            uint8_t section = partyPhase / 43;
            uint8_t rem     = (partyPhase % 43) * 6;
            switch (section % 6) {
                case 0: r=255;     g=rem;    b=0;       break;
                case 1: r=255-rem; g=255;    b=0;       break;
                case 2: r=0;       g=255;    b=rem;     break;
                case 3: r=0;       g=255-rem;b=255;     break;
                case 4: r=rem;     g=0;      b=255;     break;
                case 5: r=255;     g=0;      b=255-rem; break;
            }
            break;
        }
        case 1: {
            partyStrobeCount++;
            if (partyStrobeCount % 3 == 0) {
                uint8_t pick = random(0, 6);
                switch (pick) {
                    case 0: r=255; g=0;   b=0;   break;
                    case 1: r=0;   g=255; b=0;   break;
                    case 2: r=0;   g=0;   b=255; break;
                    case 3: r=255; g=255; b=0;   break;
                    case 4: r=255; g=0;   b=255; break;
                    case 5: r=0;   g=255; b=255; break;
                }
            } else {
                r = g = b = 0;
            }
            break;
        }
        case 2: {
            uint8_t brightness = (partyBeat < 128) ? partyBeat * 2 : (255 - partyBeat) * 2;
            uint8_t hue = partyPhase;
            uint8_t sec = hue / 43;
            uint8_t rm  = (hue % 43) * 6;
            uint8_t hr = 0, hg = 0, hb = 0;
            switch (sec % 6) {
                case 0: hr=255;    hg=rm;    hb=0;      break;
                case 1: hr=255-rm; hg=255;   hb=0;      break;
                case 2: hr=0;      hg=255;   hb=rm;     break;
                case 3: hr=0;      hg=255-rm;hb=255;    break;
                case 4: hr=rm;     hg=0;     hb=255;    break;
                case 5: hr=255;    hg=0;     hb=255-rm; break;
            }
            r = (uint8_t)((hr * brightness) / 255);
            g = (uint8_t)((hg * brightness) / 255);
            b = (uint8_t)((hb * brightness) / 255);
            break;
        }
        case 3: {
            uint8_t step = (partyPhase / 20) % 3;
            switch (step) {
                case 0: r=255; g=0;   b=0;   break;
                case 1: r=0;   g=255; b=0;   break;
                case 2: r=0;   g=0;   b=255; break;
            }
            break;
        }
        case 4: {
            uint8_t snap = (partyPhase / 30) % 3;
            switch (snap) {
                case 0: r=255; g=255; b=0;   break;
                case 1: r=255; g=0;   b=255; break;
                case 2: r=0;   g=255; b=255; break;
            }
            break;
        }
        case 5: {
            r = random(100, 256);
            g = random(100, 256);
            b = random(100, 256);
            break;
        }
        default: r=g=b=128; break;
    }

    analogWrite(RED_PIN,   255 - (r * MY_RGB_BRIGHTNESS / 255));
    analogWrite(GREEN_PIN, 255 - (g * MY_RGB_BRIGHTNESS / 255));
    analogWrite(BLUE_PIN,  255 - (b * MY_RGB_BRIGHTNESS / 255));
}

void updateRGB(unsigned long now) {
    if (partyMode) {
        updatePartyMode(now);
        return;
    }

    bool wantRed     = false;
    bool wantGreen   = false;
    bool wantBlue    = false;
    bool shouldBlink = false;
    unsigned long blinkPeriod = 2000UL;

    if (now < purpleEndTime) {
        setRGB(true, false, true);
        blinkState = false;
        return;
    }

    if (isClose) {
        wantRed      = true;
        shouldBlink  = true;
        blinkPeriod  = 300UL;
    }
    else if (now < momentaryGreenEndTime) {
        wantGreen = true;
    }
    else {
        if (!systemActive) {
            wantRed     = true;
            shouldBlink = true;
            blinkPeriod = 2000UL;
        }
        else {
            bool anyManual = relay2.manualMode || relay3.manualMode;
            if (anyManual) {
                wantRed   = true;
                wantGreen = true;
            } else {
                wantGreen = true;
            }
        }
    }

    if (shouldBlink) {
        unsigned long onTime  = blinkPeriod / 2;
        unsigned long offTime = blinkPeriod - onTime;

        if (now - lastBlinkToggle >= (blinkState ? onTime : offTime)) {
            blinkState      = !blinkState;
            lastBlinkToggle = now;
        }

        if (blinkState) setRGB(wantRed, wantGreen, wantBlue);
        else            turnRGBOff();
    } else {
        setRGB(wantRed, wantGreen, wantBlue);
        blinkState = false;
    }
}

void handleButtons(unsigned long now) {
    if (manualLock) return;

    int btn = readButton();
    if (btn == 0 || now - lastPressTime < BUTTON_COOLDOWN) return;

    lastPressTime = now;
    toneManager.playTone(1500, 200);

    bool changed = true;

    switch (btn) {
        case 1:
            relay1.state = !relay1.state;
            prefs.putBool("r1_state", relay1.state);
            Serial.println("Relay 1 toggled");
            break;

        case 2: {
            FanMode oldMode = fanMode;
            if      (fanMode == MANUAL_OFF) fanMode = MANUAL_ON;
            else if (fanMode == MANUAL_ON)  fanMode = AUTO;
            else                            fanMode = MANUAL_OFF;
            relay2.manualMode = (fanMode != AUTO);
            if (fanMode == AUTO && oldMode != AUTO) resetAutoStates();
            prefs.putUChar("fan_mode", (uint8_t)fanMode);
            Serial.print("Fan mode → ");
            Serial.println(getFanModeString(fanMode));
            break;
        }

        case 3: {
            bool oldManual = relay3.manualMode;
            relay3.manualMode = !relay3.manualMode;
            if (relay3.manualMode) relay3.state = !relay3.state;
            if (!relay3.manualMode && oldManual) resetAutoStates();
            prefs.putBool("r3_manual", relay3.manualMode);
            Serial.print("Outdoor light → ");
            Serial.println(relay3.manualMode ? "MANUAL" : "AUTO");
            break;
        }

        case 4: {
            relay4.state = !relay4.state;
            pirDesiredOn = relay4.state;
            prefs.putBool("r4_state", relay4.state);
            Serial.println("Indoor light toggled (button)");
            break;
        }

        case 5:
            if (systemActive) {
                partyMode = !partyMode;
                Serial.println(partyMode ? "PARTY MODE ON" : "Party mode OFF");
                toneManager.playTone(partyMode ? 1800 : 1000, 300);
            } else {
                Serial.println("System is OFF – cannot start party");
                changed = false;
            }
            break;

        default:
            changed = false;
            break;
    }

    if (changed) {
        saveSystemState();
        applyRelays();
        publishStatus();
    }
}