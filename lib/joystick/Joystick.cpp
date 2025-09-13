#include "Joystick.h"

Joystick::Joystick(int pinX, int pinY, int pinBtn, int center, int deadzone)
    : pinX(pinX), pinY(pinY), pinBtn(pinBtn), center(center), deadzone(deadzone),
      xVal(0), yVal(0), btn(false) {
    pinMode(pinBtn, INPUT_PULLDOWN);
}

void Joystick::read() {
    int rawX = analogRead(pinX);
    int rawY = analogRead(pinY);

    xVal = 0;
    yVal = 0;

    if (abs(rawX - center) > deadzone)
        xVal = map(rawX - center, -2048, 2047, -255, 255);

    if (abs(rawY - center) > deadzone)
        yVal = map(rawY - center, -2048, 2047, -255, 255);

    btn = (digitalRead(pinBtn) == LOW);
}

int Joystick::getX() const { return xVal; }
int Joystick::getY() const { return yVal; }
bool Joystick::isPressed() const { return btn; }
