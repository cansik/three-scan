//
// Created by Florian Bruggisser on 23.11.18.
//

#ifndef THREE_SCAN_THREESCANMEASUREMENT_H
#define THREE_SCAN_THREESCANMEASUREMENT_H


#include <cstdint>
#include "Vector3.h"

class Vertex {
    float azimuthalAngle = 0.0f; // degrees by lidar θ
    float polarAngle = 0.0f; // degrees by servo φ
    uint16_t radius = 0; // cm (rho)
    uint8_t signalStrength = 0;

public:
    Vertex(float azimuthalAngle, float polarAngle, uint16_t radius, uint8_t signalStrength);

    float getAzimuthalAngle() const;

    float getPolarAngle() const;

    uint16_t getRadius() const;

    uint8_t getSignalStrength() const;

    Vector3f getSphericalPosition() const;
};


#endif //THREE_SCAN_THREESCANMEASUREMENT_H
