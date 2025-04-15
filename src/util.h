#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>

struct Vector{
    int32_t x;
    int32_t y;
    int32_t z;
};

struct vector16_t{
    int16_t x;
    int16_t y;
    int16_t z;
};

struct vectorf_t{
    float x;
    float y;
    float z;
};


#endif //UTIL_H