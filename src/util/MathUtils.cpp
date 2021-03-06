//
// Created by Florian on 06.12.17.
//

#include <Arduino.h>
#include "MathUtils.h"

MathUtils::MathUtils() = default;

float MathUtils::map(float value, float minIn, float maxIn, float minOut, float maxOut) {
    return (value - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut;
}

float MathUtils::limit(float value, float min, float max) {
    return (value < max ? value : max) > min ? value : min;
}

float MathUtils::windowedSine(float x) {
    if (x < 0.0f || x > 1.0f)
        return 0.0f;

    // calculate sine
    return static_cast<float>(0.5 * (1 + sin(2 * PI * x - (PI / 2))));
}

bool MathUtils::isRandomCalled(float factor) {
    return (random(0, 1000) / 1000.0) > factor;
}

unsigned long MathUtils::millisToSeconds(unsigned long millis) {
    return millis / 1000;
}

unsigned long MathUtils::secondsToMillis(unsigned long seconds) {
    return seconds * 1000;
}

float MathUtils::degToRadf(float degree) {
    return (degree * 71) / 4068.0f;
}

float MathUtils::round_n(float num, int dec) {
    double m = (num < 0.0) ? -1.0 : 1.0;   // check if input is negative
    double pwr = pow(10, dec);
    return static_cast<float>(float(floor((double) num * m * pwr + 0.5) / pwr) * m);
}

float MathUtils::round_to(float num, float floorNumber, float ceilNumber) {
    float delta = abs(ceilNumber - floorNumber);

    if (delta * 0.5 >= abs(num))
        return ceilNumber;
    return floorNumber;
}

bool MathUtils::inRange(float num, float low, float high) {
    return (num >= low && num < high);
}

String MathUtils::timeStampString(unsigned long t) {
    auto h = t / 1000 / 60 / 60;
    auto m = (t - (h * 1000 * 60 * 60)) / 1000 / 60;
    auto s = (t - (h * 1000 * 60 * 60) - (m * 1000 * 60)) / 1000;

    return String(h) + ":" + String(m) + ":" + String(s);
}
