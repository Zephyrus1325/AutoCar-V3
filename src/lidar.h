#ifndef LIDAR_H
#define LIDAR_H
// Lidar Sensor Handler
// v0.1
// Created By Marco Aurélio 01/04/2025

#include <Arduino.h>
#include "macros.h"

void lidarTask(void* param){
    Lidar lidar;

    lidar.begin();

    while(true){
        lidar.update();
    }

    // Kill task in case something goes wrong
    error("Lidar task was interrupted for some reason");
}  

class Lidar{
    public:
    void begin(){
        Serial2.begin(115200);
    }

    void update(){
        while(Serial2.available() > 0){
            uint8_t reading = Serial2.read();
            buffer[index] = reading;
            index++;
            if(buffer[SYNC0] == 0xAA && buffer[SYNC1] == 0x00){
                if(index > (buffer[MESSAGE_LEN] + 2)){
                    parseData();
                    index = 0;
                }     
            } else if(index > SYNC1){
                index = 0;
            } 

            if(index > 100){
                index = 0;
            }
            
        }
    }

    private:
    struct reading{
        uint16_t distance;  //distance / 0.25
        uint16_t angle;     //angle * 100
    };

    enum messageData{
        SYNC0 = 0,
        SYNC1 = 1,
        MESSAGE_LEN = 2,
        SYNC2 = 3,
        SYNC3 = 4,
        MESSAGE_TYPE = 5,
        SYNC4 = 6,
        PAYLOAD_LEN = 7,
        RPM = 8,
        START_ANGLE0 = 11,
        START_ANGLE1 = 12,
        PAYLOAD_START = 13
    };

    enum messageType{
        SLOW_SPEED = 0xAE,
        NORMAL = 0xAD
    };
    // Buffer used for the Serial Data
    #define NOMINAL_SAMPLES 24
    #define DATA_SIZE 3600
    uint8_t buffer[100];
    reading raw_reading[DATA_SIZE];
    // Packet reading info
    uint16_t index = 0;
    // Function to parse the data from the buffer
    void parseData(){
        if(buffer[MESSAGE_TYPE] == NORMAL){
            // Get message parameters
            uint16_t initial_angle = ((buffer[START_ANGLE0] << 8) + buffer[START_ANGLE1]);
            uint8_t total_samples = (buffer[PAYLOAD_LEN] - 5)/3;
            uint16_t angle_increment = 2250 / total_samples;
            
            // Iterate for every sample
            for(int i = 0; i < total_samples; i++){
                uint16_t
            }
        }












        if(buffer[MESSAGE_TYPE] == NORMAL){
            uint16_t initialAngle = ((buffer[START_ANGLE0] << 8) + buffer[START_ANGLE1]);
            uint8_t samples = (buffer[PAYLOAD_LEN] - 5)/3; 
            uint16_t angle_increment = initialAngle / samples;
            for(int i = 0; i < samples; i++){
                raw_reading[i].distance = (buffer[PAYLOAD_START + i * 3 + 1] << 8) + (buffer[PAYLOAD_START + i * 3 + 2]);
                raw_reading[i].angle = initialAngle + angle_increment * i;
            }
        }
    }

      bool checkCRC(){
        uint16_t crc = 0;
        uint8_t len = buffer[MESSAGE_LEN];
        for(int i = 0; i < buffer[MESSAGE_LEN]; i++){
            crc += buffer[i];
        }
        if(((crc >> 8) == buffer[len]) && ((crc & 0xff) == buffer[len + 1])){
            return true;
        }
        return false;
      }
};


#endif //LIDAR_H