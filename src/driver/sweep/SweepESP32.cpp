//
// Created by Florian Bruggisser on 19.11.18.
//

#include "SweepESP32.h"

HardwareSerial SweepSerial(1);

SweepESP32::SweepESP32(uint8_t rxPin, uint8_t txPin, uint8_t pwrPin1, uint8_t pwrPin2) : Sweep(SweepSerial) {
    this->rxPin = rxPin;
    this->txPin = txPin;

    this->pwrPin1 = pwrPin1;
    this->pwrPin2 = pwrPin2;
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
    digitalWrite(pwrPin1, HIGH);
    digitalWrite(pwrPin2, HIGH);
}

void SweepESP32::turnPowerOff() {
    digitalWrite(pwrPin1, LOW);
    digitalWrite(pwrPin2, LOW);
}

void SweepESP32::hardReset() {
    turnPowerOff();
    delay(500);
    turnPowerOn();
}

void SweepESP32::setup() {
    pinMode(pwrPin1, OUTPUT);
    pinMode(pwrPin2, OUTPUT);

    pinMode(rxPin, INPUT_PULLUP);
    pinMode(txPin, INPUT_PULLUP);

    turnPowerOff();
}


