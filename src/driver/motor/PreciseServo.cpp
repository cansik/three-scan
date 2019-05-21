#include <cmath>

//
// Created by Florian Bruggisser on 19.11.18.
//

#include <util/MathUtils.h>
#include "PreciseServo.h"

PreciseServo::PreciseServo(uint8_t devicePin) {
    this->devicePin = devicePin;
}

void PreciseServo::setup() {
    servo.attach(devicePin, -1, 0, 180, SERVO_MIN_PULSE, SERVO_MAX_PULSE); // 180° for PDI-6221MG with power
}

void PreciseServo::directMove(uint8_t angle) {
    servo.write(angle);
}

void PreciseServo::movePulse(uint16_t pulseWidth) {
    servo.writeMicroseconds(pulseWidth);
    currentPulse = pulseWidth;
}

void PreciseServo::reset() {
    movePrecise(90.0f);
}

void PreciseServo::movePrecise(float angle, bool delayForServo) {
    auto pulseBefore = currentPulse;
    auto pulse = static_cast<uint16_t>(MathUtils::map(angle, 0.0f, 180.0f, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
    movePulse(pulse);

    if (delayForServo) {
        // calculate delta
        auto deltaPulse = abs(pulseBefore - pulse);

        auto deltaDegree = MathUtils::limit(
                static_cast<float>(deltaPulse) / (SERVO_MAX_PULSE - SERVO_MIN_PULSE) * 180.0f,
                0.0f, 180.0f);
        auto millisToWait = std::lround(servoSpeedPerDegree * deltaDegree * 1000);

        delay(static_cast<uint32_t>(millisToWait));
    }
}

void PreciseServo::movePreciseAndSlow(float angle, int waitTimePerFullAngle) {
    auto targetPulse = static_cast<uint16_t>(MathUtils::map(angle, 0.0f, 180.0f, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
    int sign = targetPulse - currentPulse > 0.0f ? 1 : -1;
    auto stepPulse = static_cast<uint16_t>(MathUtils::map(1.0f, 0.0f, 180.0f, SERVO_MIN_PULSE, SERVO_MAX_PULSE) -
                                           SERVO_MIN_PULSE) * sign;

    // move slow
    while (abs(targetPulse - currentPulse) > 1.0f) {
        movePulse(currentPulse + stepPulse);
        delay(waitTimePerFullAngle);
    }

    // move to correct position
    movePrecise(angle, false);
}
