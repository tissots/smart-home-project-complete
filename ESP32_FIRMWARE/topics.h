#ifndef TOPICS_H
#define TOPICS_H

/* =========================
   BASE TOPIC
========================= */

#define MQTT_BASE_TOPIC        "iot/home"

/* =========================
   SENSOR DATA TOPICS
========================= */

#define TOPIC_TEMPERATURE      "iot/home/temperature"
#define TOPIC_HUMIDITY         "iot/home/humidity"
#define TOPIC_LIGHT            "iot/home/light"
#define TOPIC_DISTANCE         "iot/home/distance"
#define TOPIC_MOTION           "iot/home/motion"

/* =========================
   SYSTEM STATUS
========================= */

#define TOPIC_SYSTEM_STATUS    "iot/home/system/status"
#define TOPIC_MODE_STATUS      "iot/home/system/mode"

/* =========================
   CONTROL TOPICS
========================= */

#define TOPIC_FAN_CONTROL      "iot/home/control/fan"
#define TOPIC_SYSTEM_CONTROL   "iot/home/control/system"
#define TOPIC_MODE_CONTROL     "iot/home/control/mode"

/* =========================
   DEBUG / LOGGING
========================= */

#define TOPIC_DEBUG            "iot/home/debug"

#endif