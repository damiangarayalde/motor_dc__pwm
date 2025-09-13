#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "Joystick.h"
#include "MotorDriver.h"
#include "CartController.h"

// ====== WiFi Settings ======
const char* ssid     = "Meme";
const char* password = "sasha2020";
const char* udpAddress = "192.168.0.37";  // <-- change this!
const int udpPort      = 4210;

WiFiUDP udp;

// Joystick pins
#define JOY_X   34
#define JOY_Y   35
#define JOY_BTN 25

// Motor pins
#define IN1 18
#define IN2 19
#define ENA 23
#define IN3 16
#define IN4 17
#define ENB 22

// Objects
Joystick joystick(JOY_X, JOY_Y, JOY_BTN);
MotorDriver motorLeft(IN1, IN2, ENA, 0);   // channel 0
MotorDriver motorRight(IN3, IN4, ENB, 1);  // channel 1
CartController cart(motorLeft, motorRight);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
  udp.begin(udpPort);
  Serial.printf("Listening on UDP port %d...\n", udpPort);

  Serial.println("\nInitializing motors");
  motorLeft.begin();
  motorRight.begin();
}

void loop() {
  // ====== READ JOYSTICK ======
  joystick.read();
  int x   = joystick.getX();              // -255..255
  int y   = joystick.getY();              // -255..255
  int btn = joystick.isPressed() ? 1 : 0; // boolean → int
  int forward = y; // default to local joystick
  int turn    = x;

  cart.applyInput("x", x);
  cart.applyInput("y", y);
  cart.applyInput("btn", btn);
  

  // ====== SEND JSON TO PC ======
  DynamicJsonDocument doc(128);
  doc["x"] = x;
  doc["y"] = y;
  doc["btn"] = btn;

  char buffer[128];
  size_t len = serializeJson(doc, buffer);

  udp.beginPacket(udpAddress, udpPort);
  udp.write((uint8_t*)buffer, len);
  udp.endPacket();

  Serial.printf("Sent (%d bytes): %s\n", (int)len, buffer);

  // ====== RECEIVE JSON FROM PC ======
  int packetSize = udp.parsePacket();
  if (packetSize) {
    char incoming[256];
    int rlen = udp.read(incoming, sizeof(incoming) - 1);
    if (rlen > 0) incoming[rlen] = 0;

    DynamicJsonDocument recvDoc(256);
    if (deserializeJson(recvDoc, incoming) == DeserializationError::Ok) {
        for (JsonPair kv : recvDoc.as<JsonObject>()) {
            cart.applyInput(kv.key().c_str(), kv.value().as<int>());
        }
    } else {
        Serial.printf("Invalid JSON: %s\n", incoming);
    }
  }

  // ===== UPDATE MOTORS =====
  cart.updateMotors();

  delay(50);
}
