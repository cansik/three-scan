//
// Created by Florian Bruggisser on 23.11.18.
//

#include <Arduino.h>
#include <util/MathUtils.h>
#include "Vertex.h"


Vertex::Vertex(float azimuthalAngle, float polarAngle, uint16_t radius,
               uint8_t signalStrength) {
    this->azimuthalAngle = azimuthalAngle;
    this->polarAngle = polarAngle;
    this->radius = radius;
    this->signalStrength = signalStrength;
}

float Vertex::getAzimuthalAngle() const {
    return azimuthalAngle;
}

float Vertex::getPolarAngle() const {
    return polarAngle;
}

uint16_t Vertex::getRadius() const {
    return radius;
}

uint8_t Vertex::getSignalStrength() const {
    return signalStrength;
}

Vector3f Vertex::getSphericalPosition() const {
    auto theta = MathUtils::degToRadf(azimuthalAngle);
    auto phi = MathUtils::degToRadf(polarAngle);

    auto x = static_cast<float>(radius * sin(theta) * cos(phi));
    auto y = static_cast<float>(-1.0f * radius * sin(theta) * sin(phi));
    auto z = static_cast<float>(radius * cos(theta));

    return Vector3f{x, y, z};
}