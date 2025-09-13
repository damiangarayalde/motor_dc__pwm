#include <Arduino.h>
#include <WiFi.h>
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

#define IN1 18
#define IN2 19
#define ENA 23
#define IN3 16
#define IN4 17
#define ENB 22


// Objects
Joystick joystick(JOY_X, JOY_Y, JOY_BTN);
MotorDriver motorLeft(IN1, IN2, ENA, 0); // channel 0
MotorDriver motorRight(IN3, IN4, ENB, 1); // channel 1

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

  Serial.println("\nInitializing motors");
  motorLeft.begin();
  motorRight.begin();
}



void loop() {
  joystick.read();

  
  int x = joystick.getX();      // -255..255
  int y = joystick.getY();      // -255..255
  bool btn = joystick.isPressed();

  // Build message (CSV format)
  char msg[64];
  snprintf(msg, sizeof(msg), "%d,%d,%d", x, y, btn ? 1 : 0);

  // Send via UDP
  udp.beginPacket(udpAddress, udpPort);
  udp.print(msg);
  udp.endPacket();

  Serial.printf("Sent: %s\n", msg);
  
  int forward = y;  // -255..255
  int turn    = x;  // -255..255

  int leftSpeed  = constrain(forward + turn, -255, 255);
  int rightSpeed = constrain(forward - turn, -255, 255);

  // motorLeft.setSpeed(leftSpeed);
  // motorRight.setSpeed(rightSpeed);

  //Serial.printf("F:%d \t T:%d \t| L:%d \t R:%d\n", forward, turn, leftSpeed, rightSpeed);

  // if (joystick.isPressed()) {
  //   Serial.println("Button pressed!");
  // }

  delay(50);
}