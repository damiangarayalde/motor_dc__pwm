#pragma once
#include <Arduino.h>

class Joystick {
public:
    Joystick(int pinX, int pinY, int pinBtn, int center = 2048, int deadzone = 300);

    void read(); // update readings

    int getX() const;        // raw -255..255
    int getY() const;        // raw -255..255
    bool isPressed() const;  // button state

private:
    int pinX, pinY, pinBtn;
    int center, deadzone;
    int xVal, yVal;
    bool btn;
};
