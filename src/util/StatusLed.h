//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_STATUSLED_H
#define THREE_SCAN_STATUSLED_H


#include <cstdint>
#include "Timer.h"

class StatusLed {
private:
    static uint8_t ledPin;

    static bool blinking;

    static bool isOn;

    static uint8_t brightness;

    static Timer blinkTimer;

public:
    static void setup(uint8_t ledPin = 2);

    static void loop();

    static void turnOn();

    static void turnOff();

    static void blink(unsigned long rate);

    static void noBlink();

    static void setBrightness(uint8_t brightness);
};


#endif //THREE_SCAN_STATUSLED_H
