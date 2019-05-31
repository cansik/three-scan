//
// Created by Florian Bruggisser on 19.11.18.
//

#include "SweepESP32.h"

HardwareSerial SweepSerial(1);

SweepESP32::SweepESP32(uint8_t rxPin, uint8_t txPin, uint8_t controlPowerPin) : Sweep(SweepSerial) {
    this->rxPin = rxPin;
    this->txPin = txPin;

    this->controlPowerPin = controlPowerPin;
}

void SweepESP32::open() {
    hardReset();

    SweepSerial.begin(115200, SERIAL_8N1, rxPin, txPin, false);

    pinMode(rxPin, INPUT_PULLUP);
    pinMode(txPin, INPUT_PULLUP);
}

void SweepESP32::close() {
    SweepSerial.end();
    turnPowerOff();
}

void SweepESP32::turnPowerOn() {
    digitalWrite(controlPowerPin, HIGH);
    delay(50);
}

void SweepESP32::turnPowerOff() {
    digitalWrite(controlPowerPin, LOW);
}

void SweepESP32::hardReset() {
    turnPowerOff();
    delay(500);
    turnPowerOn();
}

void SweepESP32::setup() {
    pinMode(controlPowerPin, OUTPUT);

    pinMode(rxPin, INPUT_PULLUP);
    pinMode(txPin, INPUT_PULLUP);

    turnPowerOff();
}


