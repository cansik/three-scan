//
// Created by Florian Bruggisser on 29.10.18.
//

#ifndef TIL_APPSETTINGS_H
#define TIL_APPSETTINGS_H

#define THREE_SCAN_SETTINGS_VERSION 1002

struct AppSettings {
private:
    int version = THREE_SCAN_SETTINGS_VERSION;

    // scan settings
    float startAngle = 0.0f;

    float endAngle = 180.0f;

    float angleStep = 0.25f;

public:
    int getVersion() const;

    float getStartAngle() const;

    void setStartAngle(float startAngle);

    float getEndAngle() const;

    void setEndAngle(float endAngle);

    float getAngleStep() const;

    void setAngleStep(float angleStep);
};


#endif //TIL_APPSETTINGS_H
