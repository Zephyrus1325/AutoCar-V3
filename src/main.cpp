#include <Arduino.h>
#include "files.h"
#include "lidar.h"
#include "chunkHandler.h"
#include "radio.h"
#include "files.h"
#include "sensors.h"
#include "navigation.h"

TaskHandle_t lidarHandler;
TaskHandle_t radioHandler;
TaskHandle_t chunkSendHandler;
TaskHandle_t sdHandler;
TaskHandle_t motorHandler;
TaskHandle_t navHandler;

void setup(){
    // Initialize general-use libraries
    #ifdef DEBUG
        Serial.begin(115200);
    #endif
    lidar.begin();
    //TEMPORARY: DELETE LATER
    pinMode(LEFT_MOTOR_A, OUTPUT);
    pinMode(LEFT_MOTOR_B, OUTPUT);
    pinMode(LEFT_MOTOR_PWM, OUTPUT);
    pinMode(RIGHT_MOTOR_A, OUTPUT);
    pinMode(RIGHT_MOTOR_B, OUTPUT);
    pinMode(RIGHT_MOTOR_PWM, OUTPUT);    

    analogWrite(LEFT_MOTOR_PWM, 150);
    analogWrite(RIGHT_MOTOR_PWM, 150);
    digitalWrite(LEFT_MOTOR_A, LOW);
    digitalWrite(LEFT_MOTOR_B, LOW);
    digitalWrite(RIGHT_MOTOR_A, LOW);
    digitalWrite(RIGHT_MOTOR_B, LOW);
    //radioStatus = radio.begin();
    radioQueue = xQueueCreate(RADIO_QUEUE_SIZE, sizeof(radioQueueData)); // Initialize messages queue
    chunkQueue = xQueueCreate(CHUNK_QUEUE_SIZE, sizeof(chunk_data)); // Initialize messages queue
    sdQueue = xQueueCreate(SD_QUEUE_SIZE, sizeof(SDQueueMeta));  // Initialize file operations queue
    if(radioQueue == NULL){
      error("Radio Queue was not created!\n\r");
    }
    if(chunkQueue == NULL){
        error("Chunk Queue was not created!\n\r");
    }
    if(sdQueue == NULL){
        error("SD Queue was not created!\n\r");
    }
    

    xTaskCreateUniversal(radioTask,
        "Radio Task", 
        5000, 
        (void*) 1, 
        1, 
        &radioHandler,
        AUX_CORE);

    // Aux Task for sending Chunk information
    xTaskCreateUniversal(chunkSendTask,
        "Chunk Send Task", 
        2000, 
        (void*) 1, 
        1, 
        &chunkSendHandler,
        AUX_CORE);
        
    /*
    xTaskCreateUniversal(sdTask,
        "SD Task", 
        10000, 
        (void*) 1, 
        1, 
        &sdHandler,
        AUX_CORE);  
    */
        
    xTaskCreateUniversal(lidarTask,
        "Lidar Task", 
        8000, 
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

    xTaskCreateUniversal(sensorsTask,
        "Sensor Task", 
        4000, 
        (void*) 1, 
        1, 
        &navHandler,
        MAIN_CORE); 
}

void loop(){
    vTaskDelete(NULL);  // Kills Arduino Task for better Efficiency
}
