#include "CartController.h"

CartController::CartController(MotorDriver& left, MotorDriver& right)
    : motorLeft(left), motorRight(right) { }

void CartController::applyInput(const String& key, int value) {
    handleKeyValue(key, value);
}

void CartController::applyInputs(const std::map<String, int>& inputs) {
    for (auto const& [key, value] : inputs) {
        handleKeyValue(key, value);
    }
}

void CartController::handleKeyValue(const String& key, int value) {
    Serial.printf("Key: %s \t | Value: %d\n", key, value);

    if (key == "speed_percent") {
        setSpeedPercent(value / 255.0f);
    } else if (key == "omega") {
        float maxTurnRate = 0.04f; // rad/s, tune
        setAngularRate((value / 255.0f) * maxTurnRate);
    } else if (key == "toggle_pause" && value) {
        togglePause();
    } else if (key == "toggle_tracking" && value) {
        toggleTracking();
    } else if (key == "faster" && value) {
        changeSpeed(+10); // step size
    } else if (key == "slower" && value) {
        changeSpeed(-10);
    } else if (key == "disturb_left" && value) {
        applyLateralDisturbance(+5);
    } else if (key == "disturb_right" && value) {
        applyLateralDisturbance(-5);
    } else if (key == "horn_pressed") {
        state.horn = (value != 0);
        digitalWrite(BUILTIN_LED, HIGH);  // ON
    } else if (key == "horn_released") {
        state.horn = (value != 0);
        digitalWrite(BUILTIN_LED, LOW);  // OFF
    }
}

void CartController::togglePause() {
    state.paused = !state.paused;
    if (state.paused) {
        state.speed = 0.0f;
    }
    Serial.printf("Paused: %d\n", state.paused);
}

void CartController::toggleTracking() {
    state.trackingEnabled = !state.trackingEnabled;
    state.speed = 0.0f;
    state.targetSpeed = 0.0f;
    state.angularSpeed = 0.0f;
    Serial.printf("Tracking: %d\n", state.trackingEnabled);
}

void CartController::changeSpeed(int delta) {
    if (!state.paused) {
        state.targetSpeed += delta;
        state.speed = state.targetSpeed;
    }
}

void CartController::setSpeedPercent(float percent) {
    if (!state.paused) {
        float maxSpeed = 200.0f; // tune as needed
        state.targetSpeed = percent * maxSpeed;
        state.speed = state.targetSpeed;
    }
}

void CartController::setAngularRate(float omega) {
    if (!state.paused) {
        state.angularSpeed = omega;
        updateWheelSpeeds(omega);
    }
}

void CartController::applyLateralDisturbance(float displacement) {
    state.y += displacement;
}

void CartController::adjustDirection(float step) {
    state.angularSpeed += step;
    updateWheelSpeeds(state.angularSpeed);
}

void CartController::updateWheelSpeeds(float omega) {
    float d = 0.2f; // wheelbase distance (m), tune
    state.vLeft  = state.speed - 0.5f * d * omega;
    state.vRight = state.speed + 0.5f * d * omega;
}

void CartController::updateMotors() {
    int leftPWM  = constrain((int)state.vLeft,  -255, 255);
    int rightPWM = constrain((int)state.vRight, -255, 255);

    motorLeft.setSpeed(leftPWM);
    motorRight.setSpeed(rightPWM);

    Serial.printf("Speed: %.2f, Target: %.2f, Omega: %.2f | L:%d R:%d\n",
                  state.speed, state.targetSpeed, state.angularSpeed,
                  leftPWM, rightPWM);
}
