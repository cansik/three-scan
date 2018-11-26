//
// Created by Florian Bruggisser on 29.10.18.
//

#ifndef TIL_APPSETTINGS_H
#define TIL_APPSETTINGS_H

#include <cstdint>
#include <driver/sweep/Sweep.h>

#define THREE_SCAN_SETTINGS_VERSION 1002


struct AppSettings {
private:
    int version = THREE_SCAN_SETTINGS_VERSION;

    // scan settings
    float startAngle = 0.0f;

    float endAngle = 180.0f;

    float angleStep = 1.0f;

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
};


#endif //TIL_APPSETTINGS_H
