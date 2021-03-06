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

const unsigned char *AppSettings::getMotorSpeed() const {
    return motorSpeed;
}

void AppSettings::setMotorSpeed(const unsigned char *motorSpeed) {
    AppSettings::motorSpeed = motorSpeed;
}

const unsigned char *AppSettings::getSampleRate() const {
    return sampleRate;
}

void AppSettings::setSampleRate(const unsigned char *sampleRate) {
    AppSettings::sampleRate = sampleRate;
}

float AppSettings::getAngleStep() const {
    return angleStep;
}

void AppSettings::setAngleStep(float angleStep) {
    AppSettings::angleStep = angleStep;
}

unsigned int AppSettings::getSliceIterationCount() const {
    return sliceIterationCount;
}

void AppSettings::setSliceIterationCount(unsigned int sliceIterationCount) {
    AppSettings::sliceIterationCount = sliceIterationCount;
}

unsigned int AppSettings::getStandFilterSize() const {
    return standFilterSize;
}

void AppSettings::setStandFilterSize(unsigned int standFilterSize) {
    AppSettings::standFilterSize = standFilterSize;
}

unsigned int AppSettings::getMinSignalStrength() const {
    return minSignalStrength;
}

void AppSettings::setMinSignalStrength(unsigned int minSignalStrength) {
    AppSettings::minSignalStrength = minSignalStrength;
}

bool AppSettings::isSerialTransfer() const {
    return serialTransfer;
}

void AppSettings::setSerialTransfer(bool serialTransfer) {
    AppSettings::serialTransfer = serialTransfer;
}
