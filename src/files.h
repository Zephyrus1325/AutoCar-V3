#ifndef FILES_H
#define FILES_H

// SD Card Handler
// v0.1
// Created By Marco Aurélio 01/04/2025

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "macros.h"
#include "configs.h"

QueueHandle_t sdQueue;

enum operation{
    READ = 0,
    WRITE = 1,
};

// Meta data for SD card transfers
/*
    @param operation  Type of operation
    @param length  Length of data to be sent/received
    @param data  Pointer to data to be sent/changed
    @param path  Path of file to be read/written to
*/
struct SDQueueMeta{
    uint8_t operation;  // Type of operation (lol)
    size_t length;      // Length of data to be sent/received
    void* data;         // Pointer to data to be sent/changed
    char path[64];      // Path of file to be read/written to
};

// Struct used for handling SD Card Metadata
struct {
    uint8_t type;
    uint64_t size;
    uint64_t used;
} SD_Meta;


// Begin SD Card (Must be called before using SD Card)
void SD_begin(){

    if(!SD.begin(SD_PIN)){
        error("SD Card Mount Failed\n");
        return;
    }

    SD_Meta.type = SD.cardType();

    if(SD_Meta.type == CARD_NONE){
      error("No SD card attached\n");
      return;
    }

    SD_Meta.size = SD.totalBytes();
    SD_Meta.used = SD.usedBytes();
    
}

// Reads a block of data from SD card and writes to buffer
void readSD(const char* path, uint8_t* buffer, uint32_t data_length){
    File file = SD.open(path, FILE_READ);
    

    if(!file){
        error("Failed to open file for reading\n");
        return;
    }

    if(!file.read(buffer, data_length)){
        error("Data reading failed!\n");
    }

    file.close();
}

// Writes a block of data from buffer to SD card
void writeSD(const char* path, uint8_t* buffer, uint32_t data_length){
    File file = SD.open(path, FILE_WRITE);

    if(!file){
        error("Failed to open file for writing\n");
        return;
    }

    if(!file.write(buffer, data_length)){
        error("File write failed!\n");
    }

    file.close();
}

void sdTask(void* param){
    while(true){
        esp_task_wdt_reset();
        SDQueueMeta buffer;
        xQueueReceive(sdQueue, &buffer, portMAX_DELAY);
        if(buffer.operation == WRITE){
            writeSD(buffer.path, (uint8_t*) buffer.data, buffer.length);
        } else {
            readSD(buffer.path, (uint8_t*) buffer.data, buffer.length);
        }
        yield();
    }
}

#endif // FILES_H