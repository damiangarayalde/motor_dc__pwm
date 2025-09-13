#include <Arduino.h>
#include "Joystick.h"
#include "MotorDriver.h"
#include "CartController.h"
#include "CommModule.h"

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
CommModule comm(udpAddress, udpPort);

void setup() {
  Serial.begin(115200);
  
  motorLeft.begin();
  motorRight.begin();

  comm.begin(ssid, password);
}

void loop() {
  // ====== READ JOYSTICK ======
  // joystick.read();
  // int x   = joystick.getX();              // -255..255
  // int y   = joystick.getY();              // -255..255
  // int btn = joystick.isPressed() ? 1 : 0; // boolean → int
  // int forward = y; // default to local joystick
  // int turn    = x;

  // cart.applyInput("x", x);
  // cart.applyInput("y", y);
  // cart.applyInput("btn", btn);
  

  // ====== SEND JSON TO PC ======
  // std::map<String, int> inputs;
  // inputs["x"] = x;
  // inputs["y"] = y;
  // inputs["btn"] = btn;

  // char buffer[128];
  // comm.send(inputs);

  // ====== RECEIVE JSON FROM PC ======
  std::map<String, int> remoteInputs = comm.receive();
  for (auto const& [key, value] : remoteInputs) {
      cart.applyInput(key, value);
  }

  // ===== UPDATE MOTORS =====
  cart.updateMotors();

  delay(50);
}
