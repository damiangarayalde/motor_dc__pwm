#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "Joystick.h"
#include "MotorDriver.h"

// ====== WiFi Settings ======
const char* ssid     = "Meme";
const char* password = "sasha2020";

// Send to Python host (replace with your PC's IP)
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
    DeserializationError error = deserializeJson(recvDoc, incoming);

    if (!error) {
        // Extract values from received JSON
        if (recvDoc.containsKey("x")) turn = recvDoc["x"].as<int>();
        if (recvDoc.containsKey("y")) forward = recvDoc["y"].as<int>();
        if (recvDoc.containsKey("btn")) {
            int btnRemote = recvDoc["btn"].as<int>();
            Serial.printf("Remote Button: %d\n", btnRemote);
        }

      //  Serial.printf("Received JSON: forward=%d, turn=%d\n", forward, turn);

    } else {
        Serial.printf("Invalid JSON: %s\n", incoming);
    }
  }

  int leftSpeed  = constrain(forward + turn, -255, 255);
  int rightSpeed = constrain(forward - turn, -255, 255);

  // Uncomment when ready to drive motors:
  // motorLeft.setSpeed(leftSpeed);
  // motorRight.setSpeed(rightSpeed);

  // Debug print
  Serial.printf("F:%d \t T:%d \t| L:%d \t R:%d\n", forward, turn, leftSpeed, rightSpeed);

  delay(50);
}
