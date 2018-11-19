//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_SWEEPESP32_H
#define THREE_SCAN_SWEEPESP32_H


#include "Sweep.h"

class SweepESP32 : public Sweep {
private:
    HardwareSerial sweepSerial = HardwareSerial(1);

    uint8_t txPin;
    uint8_t rxPin;

public:
    explicit SweepESP32(uint8_t rxPin, uint8_t txPin, HardwareSerial serial = HardwareSerial(1));

    void open();

    void close();
};


#endif //THREE_SCAN_SWEEPESP32_H
