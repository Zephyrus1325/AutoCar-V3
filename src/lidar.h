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

    #define DATA_SIZE 3600
    public:

    void begin(){
        Serial2.begin(115200);          // Initialize Serial for Data Receiving
        pinMode(LIDAR_PIN, OUTPUT);     // Set Motor Control Pin as OUTPUT
        setSpeed(215);                  // Set Motor Speed
        // Allocate Memory for raw readings buffer
        raw_reading = (reading*) malloc(sizeof(reading) * DATA_SIZE);
        if (raw_reading == nullptr) {
            error("Memory Allocation Error: LIDAR\n");
        }
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
    reading* raw_reading = nullptr;
    // Packet reading info
    uint16_t index = 0;
    // Function to parse the data from the buffer
    void parseData(){
        
        uint8_t rpm = buffer[RPM]; 
        if(buffer[MESSAGE_TYPE] == NORMAL){
            // Get message parameters
            uint16_t initial_angle = ((buffer[START_ANGLE0] << 8) + buffer[START_ANGLE1]);
            uint8_t total_samples = (buffer[PAYLOAD_LEN] - 5)/3;
            uint16_t angle_increment = 2250 / total_samples;
            // Iterate for every sample
            //for(int i = 0; i < total_samples*3; i += 3){
            //    // Iterator thingy
            //    uint16_t distance = (buffer[PAYLOAD_START + i] << 8) + buffer[PAYLOAD_START + i + 1];
            //}
            
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
char text[] = "hexamryiakaipentachiliapentahectokaitriacontakaiheptagon";

void lidarTask(void* param){
    log_message msg;
    msg.setText("hexamryiakaipentachiliapentahectokaitriacontakaiheptagon");
    transmitData(LOG_MESSAGE, &msg, sizeof(log_message));

    SDQueueMeta meta{WRITE, sizeof(text), text, "/HelloWorld2.txt"};
    xQueueSend(sdQueue, &meta, 0);

    chunk_begin();

    while(true){
        esp_task_wdt_reset();               // Reset WatchDog Timer, or else task will fail
        lidar.update();                     // Update Lidar object
        vTaskDelay(pdMS_TO_TICKS(10));      // Delay a bit for other tasks to work and free CPU time
    }

    // Kill task in case something goes wrong
    error("Lidar task was interrupted for some reason\n\r");
}  


#endif //LIDAR_H