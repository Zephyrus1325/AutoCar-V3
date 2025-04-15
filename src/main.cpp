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
    radioQueue = xQueueCreate(RADIO_QUEUE_SIZE, sizeof(radioQueueData)); // Initialize messages queue
    
    xTaskCreateUniversal(radioTask,
        "Radio Task", 
        10000, 
        (void*) 1, 
        1, 
        &radioHandler,
        AUX_CORE);
        
    
    //xTaskCreateUniversal(SDTask,
    //    "SD Task", 
    //    8000, 
    //    (void*) 1, 
    //    1, 
    //    &sdHandler,
    //    AUX_CORE);  
    
        
    xTaskCreateUniversal(lidarTask,
        "Lidar Task", 
        //18000,
        16000, 
        (void*) 1, 
        1, 
        &lidarHandler,
        MAIN_CORE);
   

    //xTaskCreateUniversal(motorTask,
    //    "Motor Task", 
    //    8000, 
    //    (void*) 1, 
    //    1, 
    //    &motorHandler,
    //    MAIN_CORE); 
        
    //xTaskCreateUniversal(navTask,
    //    "Navigation Task", 
    //    8000, 
    //    (void*) 1, 
    //    1, 
    //    &navHandler,
    //    AUX_CORE); 
    vTaskStartScheduler(); // Start Running Tasks;
}

void loop(){
    vTaskDelete(NULL);  // Kills Arduino Task for better Efficiency
}
