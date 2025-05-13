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
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_LOW);
    network.begin(90, car_address);

    
    
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

void sendSerial(chunk_data* chunk){
    Serial.write(0xAA);
    Serial.write(0x00);
    Serial.write(chunk->subdivision);

    int16_t posX = (int16_t) chunk->position.x;
    int16_t posY = (int16_t) chunk->position.y;
    Serial.write(posX & 0x00ff);
    Serial.write(posX >> 8);
    Serial.write(posY & 0x00ff);
    Serial.write(posY >> 8);

    for(int i = 0; i < 128; i++){
        Serial.write(chunk->data[i]);
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
        //sendSerial(&buffer);
    
        //Serial.print(buffer.position.x);
        //Serial.print(" | ");
        //Serial.print(buffer.position.y);
        //Serial.print(" | ");
        //Serial.println(buffer.subdivision);

        if(!uxQueueMessagesWaiting(chunkQueue)){
            sendingChunk = 0;
        }
        
    }
}



#endif //RADIO_H