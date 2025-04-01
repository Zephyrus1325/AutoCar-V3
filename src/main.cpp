#include <Arduino.h>
#include "files.h"
#include "lidar.h"
#include "chunkHandler.h"
#include "radio.h"


TaskHandle_t lidarHandler;
TaskHandle_t radioHandler;

void setup(){
    // Initialize general-use libraries
    //SD_begin();
    #ifdef DEBUG
        Serial.begin(115200);
    #endif
    
    xTaskCreateUniversal(lidarTask,
         "Lidar Task", 
         18000, 
         (void*) 1, 
         1, 
         &lidarHandler,
        ARDUINO_RUNNING_CORE);

    xTaskCreateUniversal(radioTask,
        "Radio Task", 
        8000, 
        (void*) 1, 
        1, 
        &radioHandler,
        ARDUINO_RUNNING_CORE);
}

void loop(){}
