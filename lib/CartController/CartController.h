#pragma once
#include <Arduino.h>
#include "MotorDriver.h"
#include <map>
#include <string>

struct CartState {
    int forward = 0;
    int turn    = 0;
    bool btn    = false;
};

class CartController {
public:
    CartController(MotorDriver& left, MotorDriver& right);

    // Apply a generic key/value input
    void applyInput(const String& key, int value);

    // Update motor speeds based on current state
    void updateMotors();

    // Access state (optional)
    CartState getState() const { return state; }

private:
    MotorDriver& motorLeft;
    MotorDriver& motorRight;
    CartState state;

    void handleKeyValue(const String& key, int value);
};
