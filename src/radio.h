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

uint8_t transmitRole = 1;   // Start car as always transmitting

RF24Network network(radio);
// Set up main data queue

// -------------------- Data Parsing ----------------------------------------------------------------

void sendChunk(chunk_data* chunk){
    Serial.write(0xAA);
    Serial.write(0x00);
    Serial.write(chunk->subdivision);

    int16_t posX = (int16_t) chunk->position.x;
    int16_t posY = (int16_t) chunk->position.y;
    Serial.write(posX & 0x00ff);
    Serial.write(posX >> 8);
    Serial.write(posY & 0x00ff);
    Serial.write(posY >> 8);

    for(int i = 0; i < CHUNK_RADIO_SIZE; i++){
        Serial.write(chunk->data[i]);
    }
}

void parseCommand(command* comm){
    Serial.println(comm->command_type);
    switch(comm->command_type){
        case END_OF_PACKET:
            //Serial.println("RECEIVED EOP");
            transmitRole = 1;
            break;
        default:
            break;

    }
}

// -------------------- Data Transmission ----------------------------------------------------------------

void transmitData(message_type type, void* dataToSend, size_t data_length){
    radioQueueData radio_data{type};
    memcpy(radio_data.data, dataToSend, data_length);
    xQueueSend(radioQueue, &radio_data, 0);
}


void transmitData(){
    if(uxQueueMessagesWaiting(radioQueue)){
        radioQueueData data;
        xQueueReceive(radioQueue, &data, 0);
        RF24NetworkHeader header(base_address);
        network.write(header, &data, sizeof(data));
        //Serial.println("Data MSG");
    }
    
}

void receiveData(){
    if(network.available()){
        RF24NetworkHeader header;
        radioQueueData buffer;
        network.read(header, &buffer, sizeof(buffer));
        switch(buffer.messageType){
            case COMMAND:
                parseCommand((command*) &buffer.data);
                break;
            default:
                break;
        }
    }
}

void radioTask(void* param){

    if (!radio.begin()) {
        //error("RF24 Radio hardware is not responding!!\n\r");
        Serial.println("RF24 Radio hardware is not responding!!");
        vTaskDelay(portMAX_DELAY);  // wait forever
    }
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_LOW);
    network.begin(90, car_address);

    
    while(true){
        esp_task_wdt_reset();
        network.update();
        if(transmitRole){
            transmitData();
        } else {
            receiveData();
        }
        vTaskDelay(10);
    }
}



// -------------------- Transmitting Chunks Shit ----------------------------------------------------------------
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

    for(int i = 0; i < CHUNK_RADIO_SIZE; i++){
        Serial.write(chunk->data[i]);
    }
}


// Auxiliary task to handle chunk sending
uint8_t sendingChunk = 0;
void chunkSendTask(void* param){
    while(true){
        if(transmitRole){
            if(!uxQueueMessagesWaiting(chunkQueue)){
                RF24NetworkHeader endheader(base_address);
                command comm{END_OF_PACKET};
                radioQueueData radioData{COMMAND};
                memcpy(&radioData.data, &comm, sizeof(command));
                transmitRole = 0;
                sendingChunk = 0;
                while(!network.write(endheader, &radioData, sizeof(radioData))){/*Serial.println("SENDING EOP");*/ vTaskDelay(100);}
                //sendingChunk = 0;
                //Serial.println("SENT EOP");
            } else {
                RF24NetworkHeader header(base_address);
                chunk_data data;
                xQueueReceive(chunkQueue, &data, 0);
                //Serial.println("SENDING!");
                network.write(header, &data, sizeof(data));
            }

        } else {
            //Serial.println("Waiting..");
            vTaskDelay(100);
        }
    }
}



#endif //RADIO_H