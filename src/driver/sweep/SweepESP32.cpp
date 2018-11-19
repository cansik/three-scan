//
// Created by Florian Bruggisser on 19.11.18.
//

#include "SweepESP32.h"

SweepESP32::SweepESP32(uint8_t rxPin, uint8_t txPin, HardwareSerial serial) : Sweep(serial) {
    this->sweepSerial = serial;
    this->rxPin = rxPin;
    this->txPin = txPin;
}

void SweepESP32::open() {
    sweepSerial.begin(9600, SERIAL_8N1, rxPin, txPin, false);
}

void SweepESP32::close() {
    sweepSerial.end();
}


