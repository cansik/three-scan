//
// Created by Florian on 06.12.17.
//

#ifndef SILVA_FLOATUTIL_H
#define SILVA_FLOATUTIL_H


class MathUtils {
protected:
    MathUtils();

public:
    static float map(float value, float minIn, float maxIn, float minOut, float maxOut);

    static float limit(float value, float min, float max);

    static float windowedSine(float x);

    static bool isRandomCalled(float factor);

    static unsigned long secondsToMillis(unsigned long seconds);

    static unsigned long millisToSeconds(unsigned long millis);

    static float degToRadf(float degree);

    static float round_n(float num, int dec);

    static float round_to(float num, float floorNumber, float ceilNumber);

    static bool inRange(float num, float low, float high);
};


#endif //SILVA_FLOATUTIL_H
