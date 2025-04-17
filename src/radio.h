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
uint8_t* buffer = nullptr;
// Set up main data queue

void transmitData(message_type type, void* dataToSend, size_t data_length){
    radioQueueData radio_data{type};
    memcpy(radio_data.data, dataToSend, data_length);
    xQueueSend(radioQueue, &radio_data, 0);
}



void radioTask(void* param){

    if (!radio.begin()) {
        error("RF24 Radio hardware is not responding!!\n");
        while (1) {yield();}  // hold in infinite loop
    }
    
    buffer = (uint8_t*) malloc(sizeof(uint8_t) * RECEIVE_BUFFER_SIZE);
    if (buffer == nullptr) {
        error("Memory Allocation Error: Radio\n");
    }

    radio.setChannel(90);
    network.begin(car_address);

    while(true){
        esp_task_wdt_reset();
        network.update();
        //Check if there is any message inside the queue
        if(uxQueueMessagesWaiting(radioQueue)){
            radioQueueData buffer;
            xQueueReceive(radioQueue, &buffer, 0);
            RF24NetworkHeader header(base_address);
            network.write(header, &buffer, sizeof(radioQueueData));
        }
        vTaskDelay(MIN_DELAY);
    }
}

#endif //RADIO_H