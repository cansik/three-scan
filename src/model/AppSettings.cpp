//
// Created by Florian Bruggisser on 29.10.18.
//

#include "AppSettings.h"

int AppSettings::getVersion() const {
    return version;
}

float AppSettings::getStartAngle() const {
    return startAngle;
}

void AppSettings::setStartAngle(float startAngle) {
    AppSettings::startAngle = startAngle;
}

float AppSettings::getEndAngle() const {
    return endAngle;
}

void AppSettings::setEndAngle(float endAngle) {
    AppSettings::endAngle = endAngle;
}

float AppSettings::getAngleStep() const {
    return angleStep;
}

void AppSettings::setAngleStep(float angleStep) {
    AppSettings::angleStep = angleStep;
}
