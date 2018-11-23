//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_PRECISESERVO_H
#define THREE_SCAN_PRECISESERVO_H


#include <cstdint>
#include <Servo.h>

#define SERVO_MIN_PULSE 500
#define SERVO_MAX_PULSE 2570

class PreciseServo {
private:
    uint8_t devicePin;
    Servo servo;

    uint16_t currentPulse = 0;

    const double servoSpeedPerDegree = (0.18f * 3) / 180;

public:
    explicit PreciseServo(uint8_t devicePin);

    void setup();

    void reset();

    void directMove(uint8_t angle);

    void movePrecise(float angle, bool delayForServo = false);

    void movePulse(uint16_t pulseWidth);
};


#endif //THREE_SCAN_PRECISESERVO_H
