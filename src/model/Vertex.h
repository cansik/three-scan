//
// Created by Florian Bruggisser on 23.11.18.
//

#ifndef THREE_SCAN_THREESCANMEASUREMENT_H
#define THREE_SCAN_THREESCANMEASUREMENT_H


#include <cstdint>
#include "Vector3.h"

class Vertex {
    float azimuthalAngle; // degrees by lidar θ
    float polarAngle; // degrees by servo φ
    uint16_t radius; // cm (rho)
    uint8_t signalStrength;

public:
    Vertex(float azimuthalAngle, float polarAngle, uint16_t radius, uint8_t signalStrength);

    float getAzimuthalAngle() const;

    float getPolarAngle() const;

    uint16_t getRadius() const;

    uint8_t getSignalStrength() const;

    Vector3f getSphericalPosition() const;
};


#endif //THREE_SCAN_THREESCANMEASUREMENT_H
