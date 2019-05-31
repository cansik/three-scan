//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_SWEEPESP32_H
#define THREE_SCAN_SWEEPESP32_H


#include "Sweep.h"

class SweepESP32 : public Sweep {
private:
    uint8_t txPin;
    uint8_t rxPin;

    uint8_t controlPowerPin;

    void turnPowerOn();

    void turnPowerOff();

public:
    explicit SweepESP32(uint8_t rxPin, uint8_t txPin, uint8_t controlPowerPin);

    void setup();

    void open();

    void close();

    void hardReset();
};


#endif //THREE_SCAN_SWEEPESP32_H
