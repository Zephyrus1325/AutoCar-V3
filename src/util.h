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

struct {
    uint8_t navigation_mode;
    uint8_t radio_pa;
    uint8_t radio_channel;
    uint8_t radio_datarate;
    uint8_t lidar_speed;
    uint8_t motorL_mode;
    float motorL_kp;
    float motorL_ki;
    float motorL_kd;
    uint8_t motorR_mode;
    float motorR_kp;
    float motorR_ki;
    float motorR_kd;

} initialConfigs;

#endif //UTIL_H