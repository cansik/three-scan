//
// Created by Florian Bruggisser on 29.10.18.
//

#ifndef TIL_APPSETTINGS_H
#define TIL_APPSETTINGS_H

#include <cstdint>
#include <driver/sweep/Sweep.h>

#define THREE_SCAN_SETTINGS_VERSION 1006


struct AppSettings {
private:
    int version = THREE_SCAN_SETTINGS_VERSION;

    // scan settings
    float startAngle = 0.0f;

    float endAngle = 180.0f;

    float angleStep = 0.25f;

    unsigned int sliceIterationCount = 2;

    unsigned int standFilterSize = 80;

    unsigned int minSignalStrength = 50;

    bool serialTransfer = false;

    const unsigned char *motorSpeed = MOTOR_SPEED_CODE_5_HZ;

    const unsigned char *sampleRate = SAMPLE_RATE_CODE_1000_HZ;

public:
    int getVersion() const;

    float getStartAngle() const;

    void setStartAngle(float startAngle);

    float getEndAngle() const;

    void setEndAngle(float endAngle);

    const unsigned char *getMotorSpeed() const;

    void setMotorSpeed(const unsigned char *motorSpeed);

    const unsigned char *getSampleRate() const;

    void setSampleRate(const unsigned char *sampleRate);

    float getAngleStep() const;

    void setAngleStep(float angleStep);

    unsigned int getSliceIterationCount() const;

    void setSliceIterationCount(unsigned int sliceIterationCount);

    unsigned int getStandFilterSize() const;

    void setStandFilterSize(unsigned int standFilterSize);

    unsigned int getMinSignalStrength() const;

    void setMinSignalStrength(unsigned int minSignalStrength);

    bool isSerialTransfer() const;

    void setSerialTransfer(bool serialTransfer);
};


#endif //TIL_APPSETTINGS_H
