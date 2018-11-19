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
    //servo.attach(devicePin, -1, 0, 180, 500, 2500); // 180° for PDI-6221MG // 1380 => center
    //servo.attach(devicePin, -1, 0, 180, 650, 2150); // 180° for MG995R
    //servo.attach(devicePin, -1, 0, 180, 1000, 2000); // 180° correct format
}

void PreciseServo::move(uint8_t angle) {
    servo.write(angle);
}

void PreciseServo::movePulse(uint16_t pulseWidth) {
    servo.writeMicroseconds(pulseWidth);
}

void PreciseServo::reset() {
    servo.write(90);
}

void PreciseServo::movePrecise(float angle) {
    auto pulse = static_cast<uint16_t>(MathUtils::map(angle, 0.0f, 180.0f, SERVO_MIN_PULSE, SERVO_MAX_PULSE));
    servo.writeMicroseconds(pulse);
}
