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
RF24NetworkHeader received;
uint8_t buffer[RECEIVE_BUFFER_SIZE];
// Set up main data queue


void radioTask(void* param){

    if (!radio.begin()) {
        error("RF24 Radio hardware is not responding!!\n");
        while (1) {}  // hold in infinite loop
    }
    
    radio.setPALevel(RF24_PA_HIGH);      // Set radio Power Amplifier level
    radio.setChannel(40);               // Set radio frequency
    
    network.begin(car_address);         // Initialize Network protocol
    

    if(radioQueue == NULL){
      error("Radio Queue was not created!\n");
    }

    while(true){
        esp_task_wdt_reset();
        //Check if there is any message inside the queue
        if(uxQueueMessagesWaiting(radioQueue)){
            // If there is a message, process it and send it
            radioQueueData message;
            xQueueReceive(radioQueue, &message, 0);    // Read metadata from queue
            bool status = 0;                           // Keeps track of message status
            uint16_t data_length = 0;
            log("start:");                         
            log(status);
            log('\n');
            switch(message.messageType){
                case LOG_MESSAGE:
                {
                    data_length = sizeof(log_message);   
                    break;
                }
                case INFO_NAV:
                {
                    data_length = sizeof(nav_data);
                    break;
                }
                case INFO_RADIO:
                {
                    data_length = sizeof(radio_info);
                    break;
                }
                case INFO_LIDAR:
                {
                    data_length = sizeof(lidar_info);
                    break;
                }
                case INFO_GPS:
                {
                    data_length = sizeof(gps_info);
                    break;
                }
                case INFO_MOTOR:
                {
                    data_length = sizeof(motor_info);
                    break;
                }
                case INFO_CHUNK:
                    // I dunno what to put in here
                    break;

                case CHUNK_METADATA:
                {
                    data_length = sizeof(chunk_metadata));
                    break;
                }
                case CHUNK_DATA:
                {
                    data_length = sizeof(chunk_data);
                    break;
                }
                case COMMAND:
                {
                    data_length = sizeof(command);
                    break;
                }
                default:
                    error("invalid message??");
            }
            status = network.write(header, &message.data, sizeof(log_message));
            log("end:");                         
            log(status);
            log('\n');
            // Check if the message has not been sent sucessfully
            if(!status){
                // Try again with the same message
                xQueueSendToFront(radioQueue, &message, 0); // 
                error("Problem Sending Message\n");
            }

        //} else if(network.available()){
          //network.read(&received, );
        } else {
            // if there is none, yield the thread for another process
            yield();
        }

    }
}

#endif //RADIO_H