#include <Arduino.h>
#include "files.h"
#include "lidar.h"
#include "chunkHandler.h"
#include "radio.h"
#include "files.h"

TaskHandle_t lidarHandler;
TaskHandle_t radioHandler;
TaskHandle_t sdHandler;
TaskHandle_t motorHandler;
TaskHandle_t navHandler;

void setup(){
    // Initialize general-use libraries
    #ifdef DEBUG
        Serial.begin(115200);
    #endif
    SD_begin();
    xTaskCreateUniversal(lidarTask,
         "Lidar Task", 
         //18000,
         61000, 
         (void*) 1, 
         1, 
         &lidarHandler,
        ARDUINO_RUNNING_CORE);

    
    //xTaskCreateUniversal(radioTask,
    //    "Radio Task", 
    //    8000, 
    //    (void*) 1, 
    //    1, 
    //    &radioHandler,
    //    ARDUINO_RUNNING_CORE);
        
    
    //xTaskCreateUniversal(sdTask,
    //    "SD Task", 
    //    8000, 
    //    (void*) 1, 
    //    1, 
    //    &sdHandler,
    //    ARDUINO_RUNNING_CORE);  
    
    //xTaskCreateUniversal(motorTask,
    //    "Motor Task", 
    //    8000, 
    //    (void*) 1, 
    //    1, 
    //    &motorHandler,
    //    ARDUINO_RUNNING_CORE); 
        
    //xTaskCreateUniversal(navTask,
    //    "Navigation Task", 
    //    8000, 
    //    (void*) 1, 
    //    1, 
    //    &navHandler,
    //    ARDUINO_RUNNING_CORE); 
}

void loop(){
    vTaskDelete(NULL);  // Kills Arduino Task for better Efficiency
}
