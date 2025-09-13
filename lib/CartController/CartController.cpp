#include "CartController.h"

CartController::CartController(MotorDriver& left, MotorDriver& right)
    : motorLeft(left), motorRight(right) { }

void CartController::applyInput(const String& key, int value) {
    handleKeyValue(key, value);
}

void CartController::handleKeyValue(const String& key, int value) {
    if (key == "x") {
        state.turn = value;
    } else if (key == "y") {
        state.forward = value;
    } else if (key == "btn") {
        state.btn = (value != 0);
    }
    // Add more key mappings here: speed multiplier, mode toggle, etc.
}

void CartController::updateMotors() {
    int leftSpeed  = constrain(state.forward + state.turn, -255, 255);
    int rightSpeed = constrain(state.forward - state.turn, -255, 255);

    motorLeft.setSpeed(leftSpeed);
    motorRight.setSpeed(rightSpeed);

    // Optional: debug
    Serial.printf("F:%d T:%d | L:%d R:%d\n",
                  state.forward, state.turn,
                  leftSpeed, rightSpeed);
                  
}