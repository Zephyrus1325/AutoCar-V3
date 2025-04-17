#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "MPU6050.h"
#include "QMC5883LCompass.h"
#include "BMP180I2C.h"

#include "configs.h"
#include "macros.h"
#include "util.h"

#include "Wire.h"
// IMU Sensors Handler
// v0.1
// Created By Marco Aurélio 15/04/2025



class IMU{
    private:
    BMP180I2C bmp;
    MPU6050 mpu;
    QMC5883LCompass qmc;
    
    vector16_t accelRaw;
    vector16_t gyroRaw;
    vector16_t magRaw;
    int16_t pressureRaw;
    int16_t tempRaw;

    vector16_t accel;
    vector16_t gyro;
    vector16_t mag;
    float pressure;
    float temp;

    float heading;
    float pitch;
    float roll;


    public:
    IMU() : bmp(0x77){}

    void begin(){

        Wire.begin();

        // Set up mpu to allow I2C bus sharing
        Wire.beginTransmission(0x68);
        Wire.write(0x37);
        Wire.write(0x02);
        Wire.endTransmission();

        Wire.beginTransmission(0x68);
        Wire.write(0x6A);
        Wire.write(0x00);
        Wire.endTransmission();

        // Disable Sleep Mode
        Wire.beginTransmission(0x68);
        Wire.write(0x6B);
        Wire.write(0x00);
        Wire.endTransmission();

        // Check if all devices are working properly
        Wire.beginTransmission(0x68);
        if(Wire.endTransmission()){
            error("MPU Is not working!\n\r");
        }
        Wire.beginTransmission(0x0D);
        if(Wire.endTransmission()){
            error("QMC5883L Is not working!\n\r");
        }
        Wire.beginTransmission(0x77);
        if(Wire.endTransmission()){
            error("BMP180 Is not working!\n\r");
        }
        mpu.initialize();
        mpu.CalibrateAccel();
        mpu.CalibrateGyro();
        if(!mpu.testConnection()){
            error("MPU Is not working!\n\r");
        }

        qmc.init();
        qmc.setMagneticDeclination(-23, 53);
        qmc.setCalibrationOffsets(-2190, -650, 361);
        qmc.setCalibrationScales(1.0, 1.10, 1);

        bmp.begin();
        bmp.resetToDefaults();
        bmp.setSamplingMode(BMP180MI::MODE_UHR);
    }

    void update(){
        mpu.getAcceleration(&accel.x, &accel.y, &accel.z);
        mpu.getRotation(&gyro.x, &gyro.y, &gyro.z);
        tempRaw = mpu.getTemperature();
        //log(accel.x);
        //log(" | ");
        //log(accel.y);
        //log(" | ");
        //log(accel.z);
        //log("\n\r");

        magRaw.x = qmc.getX();
        magRaw.y = qmc.getY();
        magRaw.z = qmc.getZ();

        heading = qmc.getAzimuth();
        
        if(bmp.hasValue()){
            temp = bmp.getTemperature();
            pressure = bmp.getPressure();
        }
        

        bmp.measureTemperature();
        bmp.measurePressure();

        
        
    }

    
};

void sensorsTask(void* param){
    IMU imu;
    imu.begin();
    while(true){
        esp_task_wdt_reset();
        imu.update();
        vTaskDelay(pdMS_TO_TICKS(SENSOR_UPDATE_PERIOD)); // After updating all data, wait some time
    }
    
}

#endif // SENSORS_H