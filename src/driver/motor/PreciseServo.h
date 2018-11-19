//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_PRECISESERVO_H
#define THREE_SCAN_PRECISESERVO_H


#include <cstdint>
#include <Servo.h>

class PreciseServo {
private:
    uint8_t devicePin;
    Servo servo;

public:
    explicit PreciseServo(uint8_t devicePin);

    void setup();

    void reset();

    void move(uint8_t angle);

    void movePulse(uint16_t pulseWidth);
};


#endif //THREE_SCAN_PRECISESERVO_H
