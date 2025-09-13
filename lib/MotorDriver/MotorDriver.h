#pragma once
#include <Arduino.h>

class MotorDriver {
public:
    MotorDriver(int in1, int in2, int pwmPin, int pwmChannel, int freq = 1000, int resolution = 8);

    void begin();
    void setSpeed(int speed);  // -255..255
    void stop();

private:
    int in1, in2, pwmPin, pwmChannel, freq, resolution;
};
