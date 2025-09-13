#include "MotorDriver.h"

MotorDriver::MotorDriver(int in1, int in2, int pwmPin, int pwmChannel, int freq, int resolution)
    : in1(in1), in2(in2), pwmPin(pwmPin), pwmChannel(pwmChannel), freq(freq), resolution(resolution) {}

void MotorDriver::begin() {
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    ledcSetup(pwmChannel, freq, resolution);
    ledcAttachPin(pwmPin, pwmChannel);
    stop();
}

void MotorDriver::setSpeed(int speed) {
    if (speed > 0) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        ledcWrite(pwmChannel, speed);
    } else if (speed < 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        ledcWrite(pwmChannel, -speed);
    } else {
        stop();
    }
}

void MotorDriver::stop() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    ledcWrite(pwmChannel, 0);
}
