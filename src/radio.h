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

#include "navigation.h"
#include "radioEnum.h"

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);

uint16_t car_address = 01;
uint16_t base_address = 00;

RF24Network network(radio);
// Set up main data queue

void transmitData(message_type type, void* dataToSend, size_t data_length){
    radioQueueData radio_data{type};
    memcpy(radio_data.data, dataToSend, data_length);
    xQueueSend(radioQueue, &radio_data, 0);
}



void radioTask(void* param){

    if (!radio.begin()) {
        //error("RF24 Radio hardware is not responding!!\n\r");
        Serial.println("RF24 Radio hardware is not responding!!");
        vTaskDelay(portMAX_DELAY);  // wait forever
    }
    radio.setChannel(90);
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_MAX);
    network.begin(car_address);

    
    
    while(true){
        esp_task_wdt_reset();
        network.update();
        //Check if there is any message inside the queue
        radioQueueData buffer;
        xQueueReceive(radioQueue, &buffer, portMAX_DELAY);
        
        RF24NetworkHeader header(base_address);
        if(!network.write(header, &buffer, sizeof(radioQueueData))){
            xQueueSend(radioQueue, &buffer, 0);
        }
        // Handle received data
        if(network.available()){
            radioQueueData receive_buffer;
            RF24NetworkHeader receive_header;
            network.read(receive_header, &receive_buffer, sizeof(radioQueueData));
        }
        
    }
}

// Auxiliary task to handle chunk sending
uint8_t sendingChunk = 0;
void chunkSendTask(void* param){
    while(true){
        esp_task_wdt_reset();
        chunk_data buffer;
        xQueueReceive(chunkQueue, &buffer, portMAX_DELAY);
        
        RF24NetworkHeader header(base_address);
        network.write(header, &buffer, sizeof(chunk_data));
        
        if(!uxQueueMessagesWaiting(chunkQueue)){
            sendingChunk = 0;
        }
        vTaskDelay(3);
        
    }
}

#endif //RADIO_H