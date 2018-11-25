//
// Created by Florian Bruggisser on 25.11.18.
//

#include "ExponentialMovingAverage.h"

ExponentialMovingAverage::ExponentialMovingAverage(float alpha) {
    this->alpha = alpha;
}

void ExponentialMovingAverage::add(float value) {
    if (isFirstValue) {
        accumulator = value;
        return;
    }

    accumulator = static_cast<float>((alpha * value) + (1.0 - alpha) * accumulator);
}

float ExponentialMovingAverage::getValue() {
    return accumulator;
}

float ExponentialMovingAverage::getAlpha() const {
    return alpha;
}

void ExponentialMovingAverage::setAlpha(float alpha) {
    ExponentialMovingAverage::alpha = alpha;
}
