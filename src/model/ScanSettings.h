//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_SCANSETTINGS_H
#define THREE_SCAN_SCANSETTINGS_H


#include <cstdint>

struct ScanSettings {
    float startAngle = 0.0f;

    float endAngle = 180.0f;

    float angleStep = 1.0f;
};


#endif //THREE_SCAN_SCANSETTINGS_H
