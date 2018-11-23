//
// Created by Florian Bruggisser on 23.11.18.
//

#include <Arduino.h>
#include "Vertex.h"


Vertex::Vertex(float azimuthalAngle, float polarAngle, uint16_t radius,
               uint8_t signalStrength) : azimuthalAngle(azimuthalAngle),
                                         polarAngle(polarAngle), radius(radius),
                                         signalStrength(signalStrength) {}

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
    auto theta = radians(azimuthalAngle);
    auto phi = radians(polarAngle);

    auto x = static_cast<float>(radius * sin(theta) * cos(phi));
    auto y = static_cast<float>(radius * sin(theta) * sin(phi));
    auto z = static_cast<float>(radius * cos(theta));

    return Vector3f{x, y, z};
}