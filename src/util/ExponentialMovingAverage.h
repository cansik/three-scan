//
// Created by Florian Bruggisser on 25.11.18.
//

#ifndef THREE_SCAN_EXPONENTIALMOVINGAVERAGE_H
#define THREE_SCAN_EXPONENTIALMOVINGAVERAGE_H


class ExponentialMovingAverage {
private:
    float accumulator = 0;

    float alpha;

    bool isFirstValue = true;

public:
    explicit ExponentialMovingAverage(float alpha);

    void add(float value);

    float getValue();

    float getAlpha() const;

    void setAlpha(float alpha);
};


#endif //THREE_SCAN_EXPONENTIALMOVINGAVERAGE_H
