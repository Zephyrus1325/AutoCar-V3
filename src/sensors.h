#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "inertialUnit.h"

#include "configs.h"
#include "macros.h"
#include "util.h"

#include "Wire.h"
// IMU Sensors Handler
// v0.1
// Created By Marco Aurélio 15/04/2025


void sensorsTask(void* param){
    
    IMU imu;
    imu.begin();
    while(true){
        esp_task_wdt_reset();
        imu.update();
        String data = String(imu.getPitch()) + " | " + String(imu.getRoll()) + "\n\r";
        //log(data);
        //vTaskDelay(pdMS_TO_TICKS(SENSOR_UPDATE_PERIOD)); // After updating all data, wait some time
    }
    
}

#endif // SENSORS_H