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
        setSpeed(180);                  // Set Motor Speed
    }

    void update(){
        uint16_t loopCounter = 0;
        // While there is data available on the serial
        while(Serial2.available() > 0 && loopCounter < 10000){
            esp_task_wdt_reset();                   // Reset watchdog timer
            uint8_t reading = Serial2.read();       // Read received byte
            buffer[index] = reading;                // Store into buffer   

            index++;     
            // If the buffer starts with correct header, continue adding bytes accordingly
            if(buffer[SYNC0] == 0xAA && buffer[SYNC1] == 0x00){
                // If the index is more than the expected bytes to be received, parse the received data
                if(index > MESSAGE_LEN){
                    if(index >= (buffer[MESSAGE_LEN] + 2)){
                        parseData();
                        index = 0;
                    }     
                }            
                // Else, try to align the index
            } else if(buffer[index - 1] == 0xAA && buffer[index] == 0x00){
                buffer[SYNC0] = 0xAA;
                buffer[SYNC1] = 0x00;
                index = 2;
            } 
            // Safety measure: if index if bigger than max possible value, reset it
            if(index >= sizeof(buffer)){
                index = 0;
            }
            loopCounter++;
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
    uint8_t buffer[140];
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
            //Serial.println(angle);
            for(int i = 0; i < total_samples*3; i += 3){
            
                
                uint16_t distance = (buffer[PAYLOAD_START + i + 1] << 8) + buffer[PAYLOAD_START + i + 2];
                
                // if the distance is not a invalid value (not zero)
                if(distance){
                    float distance_final = distance * 0.0025f;
                    float angle_rad = radians(angle + angle_increment * (i/3));
                    
                    float distance = 0;
                    while(distance < distance_final){
                        setPoint(distance, angle_rad, 1);
                        distance += UNIT_SIZE;
                    }
                    setPoint(distance_final, angle_rad, 255);
                }
            }
            
        } else if(buffer[MESSAGE_TYPE] == WRONG_SPEED){
            //Serial.println(buffer[RPM]);
        }
    }

    void setPoint(float distance, float angle, uint8_t value){
        int16_t posX = distance * cos(angle) + navigation.position.x;
        int16_t posY = distance * sin(angle) + navigation.position.y;
        
        int16_t chunkX, chunkY;
        getChunkPos(posX, posY, &chunkX, &chunkY); // get Chunk pos
        
        uint16_t index;
        getChunkLocalIndex(posX, posY, &index);
        
        // load a chunk with the coordinates, and then set its bit;
        uint8_t chunk_id = getChunk(chunkX, chunkY);
        chunks[chunk_id].data[index] = value;
        chunks[chunk_id].lastWrite = millis();
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
    uint64_t lastChunkStore = 0;
    while(true){
        esp_task_wdt_reset();               // Reset WatchDog Timer, or else task will fail
        
        lidar.update();                     // Update Lidar object
        if(millis() - lastChunkUpload > DELAY_CHUNK_TRANSMIT && !sendingChunk){
            transmitChunk();
            sendingChunk = 1;
            lastChunkUpload = millis();
        }
        if(millis() - lastChunkStore > DELAY_CHUNK_STORE){
            //chunkUpdate();       
            lastChunkStore = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(4));      // Delay a bit for other tasks to work and free CPU time
    }

    // Kill task in case something goes wrong
    error("Lidar task was interrupted for some reason\n\r");
}  


#endif //LIDAR_H