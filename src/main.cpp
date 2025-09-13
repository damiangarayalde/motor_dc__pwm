#include <Arduino.h>
#include "Joystick.h"
#include "MotorDriver.h"

// Pin definitions
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
  motorLeft.begin();
  motorRight.begin();
}

void loop() {
  joystick.read();

  int forward = joystick.getY();  // -255..255
  int turn    = joystick.getX();  // -255..255

  int leftSpeed  = constrain(forward + turn, -255, 255);
  int rightSpeed = constrain(forward - turn, -255, 255);

  // motorLeft.setSpeed(leftSpeed);
  // motorRight.setSpeed(rightSpeed);

  Serial.printf("F:%d \t T:%d \t| L:%d \t R:%d\n", forward, turn, leftSpeed, rightSpeed);

  if (joystick.isPressed()) {
    Serial.println("Button pressed!");
  }

  delay(50);
}
