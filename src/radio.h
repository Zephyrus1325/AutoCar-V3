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
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_LOW);
    network.begin(90, car_address);

    
    while(true){
        esp_task_wdt_reset();
        network.update();

        // Handle received data
        if(network.available()){
            radioQueueData receive_buffer;
            RF24NetworkHeader receive_header;
            network.read(receive_header, &receive_buffer, sizeof(radioQueueData));
            
            Serial.println("Received!");
            
            switch(receive_buffer.messageType){
                case GO_FORWARD:
                    digitalWrite(LEFT_MOTOR_A, HIGH);
                    digitalWrite(LEFT_MOTOR_B, LOW);
                    digitalWrite(RIGHT_MOTOR_A, HIGH);
                    digitalWrite(RIGHT_MOTOR_B, LOW);
                    break;
                case GO_BACK:
                    digitalWrite(LEFT_MOTOR_A, LOW);
                    digitalWrite(LEFT_MOTOR_B, HIGH);
                    digitalWrite(RIGHT_MOTOR_A, LOW);
                    digitalWrite(RIGHT_MOTOR_B, HIGH);
                    break;
                case GO_LEFT:
                    digitalWrite(LEFT_MOTOR_A, LOW);
                    digitalWrite(LEFT_MOTOR_B, HIGH);
                    digitalWrite(RIGHT_MOTOR_A, HIGH);
                    digitalWrite(RIGHT_MOTOR_B, LOW);
                    break;
                case GO_RIGHT:
                    digitalWrite(LEFT_MOTOR_A, HIGH);
                    digitalWrite(LEFT_MOTOR_B, LOW);
                    digitalWrite(RIGHT_MOTOR_A, LOW);
                    digitalWrite(RIGHT_MOTOR_B, HIGH);
                    break;
                case STOP:
                    digitalWrite(LEFT_MOTOR_A, LOW);
                    digitalWrite(LEFT_MOTOR_B, LOW);
                    digitalWrite(RIGHT_MOTOR_A, LOW);
                    digitalWrite(RIGHT_MOTOR_B, LOW);
                    break;
                default:
                    break;
            }
        }

        //Check if there is any message inside the queue
        radioQueueData buffer;
        if(xQueueReceive(radioQueue, &buffer, 0)){
            RF24NetworkHeader header(base_address);
            if(!network.write(header, &buffer, sizeof(radioQueueData))){
                xQueueSend(radioQueue, &buffer, 0);
            }
        }
        vTaskDelay(5);
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

    for(int i = 0; i < CHUNK_RADIO_SIZE; i++){
        Serial.write(chunk->data[i]);
    }
}

// Auxiliary task to handle chunk sending
uint8_t sendingChunk = 0;
void chunkSendTask(void* param){
    while(true){
        esp_task_wdt_reset();
        network.update();
        chunk_data buffer;
        xQueueReceive(chunkQueue, &buffer, portMAX_DELAY);
        network.update();
        RF24NetworkHeader header(base_address);

        network.write(header, &buffer, sizeof(chunk_data));
        
        //sendSerial(&buffer);
    
        if(!uxQueueMessagesWaiting(chunkQueue)){
            sendingChunk = 0;
        }
    }
}



#endif //RADIO_H