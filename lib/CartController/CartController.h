#pragma once
#include <Arduino.h>
#include "MotorDriver.h"
#include <map>
#include <string>

struct CartState {
    float x = 0.0f;
    float y = 0.0f;
    float headingDeg = 0.0f;

    float speed = 0.0f;
    float targetSpeed = 0.0f;
    float angularSpeed = 0.0f;

    float vLeft = 0.0f;
    float vRight = 0.0f;

    bool paused = false;
    bool trackingEnabled = false;
    bool status_led = false;

};

class CartController {
public:
    CartController(MotorDriver& left, MotorDriver& right);

    void applyInput(const String& key, int value);
    void applyInputs(const std::map<String, int>& inputs);

    void updateMotors();

    // Control helpers
    void togglePause();
    void toggleTracking();
    void changeSpeed(int delta);
    void setSpeedPercent(float percent);
    void setAngularRate(float omega);
    void applyLateralDisturbance(float displacement);
    void adjustDirection(float step);

    CartState getState() const { return state; }

private:
    MotorDriver& motorLeft;
    MotorDriver& motorRight;
    CartState state;

    void handleKeyValue(const String& key, int value);
    void updateWheelSpeeds(float omega);
};
