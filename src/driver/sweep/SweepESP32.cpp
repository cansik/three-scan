//
// Created by Florian Bruggisser on 19.11.18.
//

#include "SweepESP32.h"

HardwareSerial SweepSerial(1);

SweepESP32::SweepESP32(uint8_t rxPin, uint8_t txPin) : Sweep(SweepSerial) {
    this->rxPin = rxPin;
    this->txPin = txPin;
}

void SweepESP32::open() {
    SweepSerial.begin(115200, SERIAL_8N1, rxPin, txPin, false);

    pinMode(rxPin, INPUT_PULLUP);
    pinMode(txPin, INPUT_PULLUP);
}

void SweepESP32::close() {
    SweepSerial.end();
}


