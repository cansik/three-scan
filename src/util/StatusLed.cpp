//
// Created by Florian Bruggisser on 19.11.18.
//

#include <esp32-hal-gpio.h>
#include "StatusLed.h"

uint8_t StatusLed::ledPin;

bool StatusLed::blinking;

bool StatusLed::isOn;

uint8_t StatusLed::brightness;

Timer StatusLed::blinkTimer = Timer(1000);

void StatusLed::setup(uint8_t ledPin) {
    StatusLed::ledPin = ledPin;

    pinMode(StatusLed::ledPin, OUTPUT);
    turnOff();
}

void StatusLed::loop() {
    if (blinking && blinkTimer.elapsed()) {
        if (isOn) {
            turnOff();
        } else {
            turnOn();
        }
    }
}

void StatusLed::turnOn() {
    setBrightness(255);
    isOn = true;
}

void StatusLed::turnOff() {
    setBrightness(0);
    isOn = false;
}

void StatusLed::setBrightness(uint8_t brightness) {
    StatusLed::brightness = brightness;

    if (brightness > 128) {
        digitalWrite(ledPin, HIGH);
    } else {
        digitalWrite(ledPin, LOW);
    }
}

void StatusLed::blink(unsigned long rate) {
    blinking = true;
    blinkTimer.setWaitTime(rate);
    blinkTimer.reset();
}

void StatusLed::noBlink() {
    blinking = false;
    turnOff();
}
