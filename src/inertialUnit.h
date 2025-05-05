// Inertial Unit Class
// Made by Marco Aurélio in 27/04/2025
// V0.2

// Makes all necessary readings of all sensors and returns the position of the car

#include <Arduino.h>
#include <Wire.h>

class IMU{
    private:
    float pitch = 0, heading = 0, roll = 0; // In radians
    float acc_pitch, acc_roll; //acc calculated angles In radians
    float mag_heading;
    float uncertaintyPitch = 4;
    float uncertaintyRoll = 4;
    float uncertaintyHeading = 4;

    int16_t raw_accX, raw_accY, raw_accZ;
    int16_t raw_gyroX, raw_gyroY, raw_gyroZ;
    int16_t raw_magX, raw_magY, raw_magZ;

    uint64_t lastRoll = 0, lastPitch = 0, lastHeading = 0;

    float accX, accY, accZ;
    float gyroX, gyroY, gyroZ;
    float gyro_calibX = 0, gyro_calibY = 0, gyro_calibZ = 0;

    const uint8_t mpu = 0x68;
    const uint8_t qmc = 0x0D;
    const uint32_t calibration_time = 3000;

    void sendI2C(uint8_t device, uint8_t reg, uint8_t value){
        Wire.beginTransmission(device);
        Wire.write(reg);
        Wire.write(value);
        Wire.endTransmission();
    }

    // Sets up main registers of mpu
    void beginMPU(){
        sendI2C(mpu, 0x37, 0x02); // Set up mpu to allow I2C bus sharing
        sendI2C(mpu, 0x6A, 0x00); // Disables mpu Master Mode
        sendI2C(mpu, 0x6B, 0x00); // Disable Sleep Mode
        sendI2C(mpu, 0x1A, 0x05); // Set up low-pass filter to 10 Hz
        sendI2C(mpu, 0x1C, 0x00); // Config accel scale to +- 2g
        sendI2C(mpu, 0x1B, 0x08); // Config gyro scale to +- 500 deg/s
    }

    // Sets up main registers of qmc
    void beginQMC(){
        sendI2C(qmc, 0x09, 0b00001101); // OSR 512 | 2G | 200 Hz | Continuous
    }

    // Sets up main registers of bmp
    void beginBMP(){
        // Not implemented yet, wait for version V0.2
    }

    void calibrate(){
        // TODO: Add a acc calibration system too, but with suspect status detection (non paralel surface);

        // Calibrate the gyro and accelerometers
        // DO NOT MOVE DURING THIS PROCESS
        float tempGyroX = 0, tempGyroY = 0, tempGyroZ = 0;
        for(uint32_t i = 0; i < calibration_time; i++){
            readMPU();
            tempGyroX += gyroX;
            tempGyroY += gyroY;
            tempGyroZ += gyroZ;
            vTaskDelay(1);
        }
        gyro_calibX = (float)tempGyroX/calibration_time;
        gyro_calibY = (float)tempGyroY/calibration_time;
        gyro_calibZ = (float)tempGyroZ/calibration_time;
    }

    void readMPU(){
        Wire.beginTransmission(mpu);
        Wire.write(0x3B);   // Set pointer to acc readings register
        Wire.endTransmission();
        Wire.requestFrom(mpu, 6);   // Request 6 bytes in sequence from mpu
        raw_accX = Wire.read() << 8 | Wire.read();
        raw_accY = Wire.read() << 8 | Wire.read();
        raw_accZ = Wire.read() << 8 | Wire.read();

        Wire.beginTransmission(mpu);
        Wire.write(0x43);   // Set pointer to gyro readings register
        Wire.endTransmission();
        Wire.requestFrom(mpu, 6);   // Request 6 bytes in sequence from mpu
        raw_gyroX = Wire.read() << 8 | Wire.read();
        raw_gyroY = Wire.read() << 8 | Wire.read();
        raw_gyroZ = Wire.read() << 8 | Wire.read();

        // Calculate the rates in normal Values
        // Gyro in deg/s
        gyroX = (float)raw_gyroX/65.5f;
        gyroY = (float)raw_gyroY/65.5f;
        gyroZ = (float)raw_gyroZ/65.5f;
        
        // Apply calibration to gyro
        gyroX -= gyro_calibX;
        gyroY -= gyro_calibY;
        gyroZ -= gyro_calibZ;

        // Acc in m/s
        accX = (float)-raw_accX/16384.f;
        accY = (float)-raw_accY/16384.f;
        accZ = (float)-raw_accZ/16384.f;

        accX +=  0.08f; // Add acc calibration X
        accY += -0.02f; // Add acc calibration Y
        accZ += -0.06f; // Add acc calibration Z

        // Calculate raw pitch and roll angles
        acc_roll = -atan2(accX, sqrt(accY*accY + accZ*accZ));
        acc_pitch = atan2(accY, sqrt(accX*accX + accZ*accZ));
        
        acc_roll = acc_roll * RAD_TO_DEG;
        acc_pitch = acc_pitch * RAD_TO_DEG;
    }

    void readQMC(){
        Wire.beginTransmission(qmc);
        Wire.write(0x00);   // Set pointer to acc readings register
        Wire.endTransmission();
        Wire.requestFrom(qmc, 6);   // Request 6 bytes in sequence from mpu
        raw_magX = Wire.read() | Wire.read() << 8;
        raw_magY = Wire.read() | Wire.read() << 8;
        raw_magZ = Wire.read() | Wire.read() << 8;

        raw_magX += -29700; // apply mag offset    
        raw_magY += -3900; // apply mag offset
        raw_magZ += 0; // apply mag offset  
        
        raw_magX *= 1; // apply mag gain      
        raw_magY *= 1; // apply mag gain
        raw_magZ *= 1; // apply mag gain    
        
        mag_heading = atan2(raw_magX, raw_magY) * RAD_TO_DEG;
    }

    void kalmanFilter(float* actualState, float* uncertainty, float input, float measurement, uint64_t* lastTime){

        float dt = (float)(millis() - *lastTime) / 1000.0f;

        *actualState = *actualState + dt * input;
        *uncertainty = *uncertainty + dt * dt * 4 * 4;    // Keep a eye in these "magic numbers"
        float gain = *uncertainty / (*uncertainty + 3 * 3);
        *actualState = *actualState + gain * (measurement - *actualState);
        *uncertainty = (1 - gain) * *uncertainty;

        *lastTime = millis();
    }

    public:
    IMU(){}
    void begin(){
        Wire.begin(); // Starts Wire Library
        beginMPU();   // Must run first
        beginQMC();
        beginBMP();
        calibrate();
    }
    void update(){
        readMPU();
        readQMC();
        kalmanFilter(&roll, &uncertaintyRoll, gyroY, acc_roll, &lastRoll);
        kalmanFilter(&pitch, &uncertaintyPitch, gyroX, acc_pitch, &lastPitch);
        kalmanFilter(&heading, &uncertaintyHeading, gyroZ, mag_heading, &lastHeading);
        //Serial.print(mag_heading);
        //Serial.print(" | ");
        //Serial.print(heading);
        //Serial.print(" | ");
        //Serial.print(raw_magX);
        //Serial.print(" | ");
        //Serial.print(raw_magY);
        //Serial.print(" | ");
        //Serial.println(raw_magZ);
        vTaskDelay(5);
    }

    float getPitch(){return pitch ;}    // Returns pitch in degrees
    float getHeading(){return heading;}    // Returns heading in degrees
    float getRoll(){return roll;}  // Returns roll in degrees

};