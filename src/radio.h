#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "RF24.h"
#include "RF24Network.h"
#include <SPI.h>
#include "macros.h"


#include "radioEnum.h"

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);

uint16_t car_address = 01;
uint16_t base_address = 00;

RF24Network network(radio);
RF24NetworkHeader header(base_address);

// Set up main data queue
QueueHandle_t radioQueue;

void radioTask(void* param){

    if (!radio.begin()) {
        error("RF24 Radio hardware is not responding!!");
        while (1) {}  // hold in infinite loop
    }
    
    radio.setPALevel(RF24_PA_HIGH);      // Set radio Power Amplifier level
    radio.setChannel(40);               // Set radio frequency
    
    network.begin(car_address);         // Initialize Network protocol
    radioQueue = xQueueCreate(RADIO_QUEUE_SIZE, sizeof(radioQueueMeta)); // Initialize messages queue

    while(true){
        esp_task_wdt_reset();
        //Check if there is any message inside the queue
        if(uxQueueMessagesWaiting(radioQueue)){
            // If there is a message, process it and send it
            radioQueueMeta message;
            xQueueReceive(radioQueue, &message, 0);    // Read metadata from queue
            bool status = 0;                           // Keeps track of message status
            switch(message.messageType){
                case LOG_MESSAGE:
                    String* log_message = (String*) message.data;
                    status = network.write(header, log_message->c_str(), log_message->length());
                    break;

                case INFO_NAV:
                    nav_data* navData = (nav_data*) message.data;
                    status = network.write(header, navData, sizeof(nav_data));
                    break;

                case INFO_RADIO:
                    radio_info* radioInfo = (radio_info*) message.data;
                    status = network.write(header, radioInfo, sizeof(radio_info));
                    break;

                case INFO_LIDAR:
                    lidar_info* lidarInfo = (lidar_info*) message.data;
                    status = network.write(header, lidarInfo, sizeof(lidar_info));
                    break;

                case INFO_GPS:
                    gps_info* gpsInfo = (gps_info*) message.data;
                    status = network.write(header, gpsInfo, sizeof(gps_info));
                    break;

                case INFO_MOTOR:
                    motor_info* motorInfo = (motor_info*) message.data;
                    status = network.write(header, motorInfo, sizeof(motor_info));
                    break;

                case INFO_CHUNK:
                    // I dunno what to put in here
                    break;

                case CHUNK_METADATA:
                    chunk_metadata* chunkMeta = (chunk_metadata*) message.data;
                    status = network.write(header, chunkMeta, sizeof(chunk_metadata));
                    break;

                case CHUNK_DATA:
                    chunk_data* chunkData = (chunk_metadata*) message.data;
                    status = network.write(header, chunkData, CHUNK_RADIO_SIZE);
                    break;

                case COMMAND:
                    command* comm = (command*) message.data;
                    status = network.write(header, comm, sizeof(command));
                    break;

                default:
                    error("invalid message??");
            }
            
            // Check if the message has been sent sucessfully
            if(status){
                // If succesful, set sent flag to true 
                *message.sent = true;
            } else {
                // Else, try again with the same message
                xQueueSendToFront(radioQueue, &message, 0); // 
                error("Problem Sending Message");
            }

        } else {
            // if there is none, yield the thread for another process
            yield();
        }
    }
}

#endif //RADIO_H