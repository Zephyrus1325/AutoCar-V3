#ifndef LIDAR_H
#define LIDAR_H
// Lidar Sensor Handler
// v0.1
// Created By Marco Aurélio 01/04/2025

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "macros.h"

#include "radio.h"
#include "files.h"

#include "chunkHandler.h"


class Lidar{
    public:

    void begin(){
        Serial2.begin(115200);          // Initialize Serial for Data Receiving
        pinMode(LIDAR_PIN, OUTPUT);     // Set Motor Control Pin as OUTPUT
        setSpeed(215);                  // Set Motor Speed
    }

    void update(){

        // While there is data available on the serial
        while(Serial2.available() > 0){
            esp_task_wdt_reset();                   // Reset watchdog timer
            uint8_t reading = Serial2.read();       // Read received byte
            buffer[index] = reading;                // Store into buffer
            index++;                                
            // If the buffer starts with correct header, continue adding bytes accordingly
            if(buffer[SYNC0] == 0xAA && buffer[SYNC1] == 0x00){
                // If the index is more than the expected bytes to be received, parse the received data
                if(index > (buffer[MESSAGE_LEN] + 2)){
                    parseData();
                    index = 0;
                }     
                // Else, if index is 
            } else if(index > SYNC1){
                index = 0;
            } 
            // Safety measure: if index if bigger than max possible value, reset it
            if(index >= sizeof(buffer)){
                index = 0;
            }
            
        }
        
    }

    private:
    struct reading{
        uint16_t distance;  // distance in cm = distance * 0.25
        uint16_t angle;     // angle in deg = angle / 100
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
        WRONG_SPEED = 0xAE,
        NORMAL = 0xAD
    };

    // Buffer used for the Serial2 Data
    uint8_t buffer[100];
    // Packet reading info
    uint16_t index = 0;
    // Function to parse the data from the buffer
    void parseData(){
        
        uint8_t rpm = buffer[RPM]; 
        if(buffer[MESSAGE_TYPE] == NORMAL){
            // Get message parameters
            uint16_t initial_angle = ((buffer[START_ANGLE0] << 8) + buffer[START_ANGLE1]);
            uint8_t total_samples = (buffer[PAYLOAD_LEN] - 5)/3;
            float angle_increment = 22.5f / total_samples;
            // Iterate for every sample
            float angle = initial_angle / 100.0f;
            
            for(int i = 0; i < total_samples*3; i += 3){
            
                float angle_rad = radians(angle + angle_increment * (i/3));
                uint16_t distance = (buffer[PAYLOAD_START + i + 1] << 8) + buffer[PAYLOAD_START + i + 2];
                
                // if the distance is not a invalid value (not zero)
                if(distance){
                    float distance_final = distance * 0.0025f;
                    int16_t posX = distance_final * sin(angle_rad);
                    int16_t posY = distance_final * cos(angle_rad);

                    // Find chunk of coordinate
                    
                    int16_t chunkX = posX / CHUNK_SIZE;
                    int16_t chunkY = posY / CHUNK_SIZE;
                    
                    uint16_t verticalOffset = (posY % CHUNK_SIZE) * CHUNK_SIZE;  // Finds of lateral offset for the coordinate
                    uint16_t byteIndex = (posX % CHUNK_SIZE) / 8; // Finds which byte the coordinate is
                    uint8_t bitIndex = (posX % CHUNK_SIZE) % 8;   // Finds which bit the coordinate is

                    uint16_t final_index = verticalOffset + byteIndex;
                    
                    // load a chunk with the coordinates, and then set its bit;
                    uint8_t chunk_id = getChunk(chunkX, chunkY);
                    chunks[chunk_id].data[final_index] |= (1 << bitIndex); 
                    chunks[chunk_id].lastWrite = millis();
                    
                }
            }
            
        }
    }


    // Error checking via CRC
    bool checkCRC(){
      uint16_t crc = 0;
      uint8_t len = buffer[MESSAGE_LEN];

      // Calculates CRC Sum via received data 
      for(int i = 0; i < buffer[MESSAGE_LEN]; i++){
          crc += buffer[i];
      }

      // Compares with received CRC
      if(((crc >> 8) == buffer[len]) && ((crc & 0xff) == buffer[len + 1])){
        return true;
      }
      return false;
    }

    // Sets lidar speed
    void setSpeed(uint8_t power){
        analogWrite(LIDAR_PIN, constrain(power, 0, 255));
    }
};

Lidar lidar;

void lidarTask(void* param){

    chunk_begin();
    uint64_t lastChunkUpload = 0;
    while(true){
        esp_task_wdt_reset();               // Reset WatchDog Timer, or else task will fail
        lidar.update();                     // Update Lidar object
        if(millis() - lastChunkUpload > DELAY_CHUNK_DATA){
            //transmitChunk();
            chunkUpdate();
            lastChunkUpload = millis();
        }
        vTaskDelay(pdMS_TO_TICKS(10));      // Delay a bit for other tasks to work and free CPU time
    }

    // Kill task in case something goes wrong
    error("Lidar task was interrupted for some reason\n\r");
}  


#endif //LIDAR_H