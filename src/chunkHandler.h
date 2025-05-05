#ifndef CHUNK_HANDLER_H
#define CHUNK_HANDLER_H

#include <Arduino.h>
#include "configs.h"
#include "util.h"
#include "files.h"

#include "radioEnum.h"
#include "radio.h"

// In this approach, every bit is a position, that can be either:
// 0: Free Space
// 1: Wall
struct Chunk{
    uint8_t exists;                  // Checks if a chunk file exists already or not
    Vector position;                 // Position of a chunk
    uint64_t lastWrite;              // last Time this chunk was written to
    uint64_t lastSave;               // last Time the chunk was saved to sd card
    uint8_t status;                  // Status of a chunk 0 - Unloaded | 1 - loaded | 2 - 
    uint8_t data[CHUNK_STORAGE];     // compacted data of a chunk
};

enum chunk_status{
    UNLOADED = 0,
    LOADED
};

Chunk chunks[9];    // 4 Chunks will be used, in theory, there will be one for each corner around the car


/*
    @brief
    Load a chunk into the chunks array
    @param x chunk X position
    @param y chunk Y position

*/

uint8_t loadChunk(int16_t x, int16_t y){
    // Check if chunk is already loaded
    // (Gotta keep cpu usage low)
    uint64_t lowest_time = 0xFFFFFFFFFFFFFFFF;
    uint8_t lowest_id;
    for(int i = 0; i < 9; i++){
        if(chunks[i].position.x == x && chunks[i].position.y == y && chunks[i].exists){
            return i; // This chunk is already loaded, ignore the loading
        }
        // else, find the one with oldest interaction
        if(chunks[i].lastWrite < lowest_time){
            lowest_time = chunks[i].lastWrite;
            lowest_id = i;
        }
    }
    // After finding the most optimal chunk to change, change the chunk to the new one
    // Poor cpu
    chunks[lowest_id].status = UNLOADED; // Set chunk status to unloaded to invalidate it
    // Store actual chunk data to sd card
    String path_write = "/chunks/" + String(chunks[lowest_id].position.x) + "_" + String(chunks[lowest_id].position.y) + ".chunk";
    String path_read = "/chunks/" + String(x) + "_" + String(y) + ".chunk";
    
    SDQueueMeta sd_request{WRITE, CHUNK_STORAGE, chunks[lowest_id].data, ""};
    
    // If the loaded chunk exists (mainly for set up case)
    if(chunks[lowest_id].exists){
        strncpy(sd_request.path, path_write.c_str(), path_write.length()+1);
        xQueueSend(sdQueue, &sd_request, 0);
        vTaskDelay(20);     // Delay because I cant make it faster in my mind
    }
    

    // Then read new data from sd card
    sd_request.operation = READ;
    strncpy(sd_request.path, path_read.c_str(), path_read.length()+1);
    //sd_request.path = "/chunks/none2.txt";
    xQueueSend(sdQueue, &sd_request, 0);
    vTaskDelay(20);     // Delay because I cant make it faster in my mind
    // If the chunk read does not exist, then create it
    if(!chunks[lowest_id].exists){
        chunks[lowest_id].exists = 1;
        chunks[lowest_id].position.x = x;
        chunks[lowest_id].position.y = y;
    }
    chunks[lowest_id].lastSave = millis();
    chunks[lowest_id].lastWrite = millis();
    chunks[lowest_id].status = LOADED; // Set chunk status to unloaded to invalidate it
    return lowest_id;
}


// return id of a specific chunk
// if chunk is not loaded, load it and return its id
uint8_t getChunk(int16_t x, int16_t y){
    for(int i = 0; i < 9; i++){
        if(chunks[i].position.x == x && chunks[i].position.y == y){
            return i;
        }
    }
    return loadChunk(x, y);
}

void transmitChunk(){
    for(int i = 0; i < 9; i++){
        for(uint16_t j = 0; j < CHUNK_SUBDIVISION; j++){
            chunk_data data;
            data.position = chunks[i].position;
            data.subdivision = j;
            memcpy(data.data, &chunks[i].data[j*CHUNK_SUBDIVISION], CHUNK_RADIO_SIZE);
            xQueueSend(chunkQueue, &data, 0);
        }
    }
}

void chunkUpdate(){

    // Checks if a chunk must be saved to memory
    for(int i = 0; i < 9; i++){
        if(millis() - chunks[i].lastSave > CHUNK_SAVE_TIME){
            String path_write = "/chunks/" + String(chunks[i].position.x) + "_" + String(chunks[i].position.y) + ".chunk";
            SDQueueMeta sd_request{WRITE, CHUNK_STORAGE, chunks[i].data, ""};
            strncpy(sd_request.path, path_write.c_str(), path_write.length()+1);
            xQueueSend(sdQueue, &sd_request, 0);
        }
    }
}

void getChunkPos(int16_t posX, int16_t posY, int16_t* chunkX, int16_t* chunkY){
    // Find chunk of coordinate
    int8_t constX = posX > 0 ? 1 : -1;
    int8_t constY = posY > 0 ? 1 : -1;

    *chunkX = ((posX/(CHUNK_SIZE*UNIT_SIZE/2)) + constX) / 2;
    *chunkY = ((posY/(CHUNK_SIZE*UNIT_SIZE/2)) + constY) / 2;
}

void getChunkLocalIndex(int16_t posX, int16_t posY, uint16_t* index){
    // Calculating local chunk position
    int16_t offsetX = (posX + (CHUNK_SIZE*UNIT_SIZE/2)) / UNIT_SIZE;
    int16_t offsetY = (posY + (CHUNK_SIZE*UNIT_SIZE/2)) / UNIT_SIZE;

    offsetX = offsetX > 0 ? offsetX : offsetX - 1; // Corrects the 1 offset at x < 0, at the cost of x = 0 being invalid
    offsetY = offsetY > 0 ? offsetY : abs(offsetY) -  CHUNK_SIZE * (abs(offsetY/CHUNK_SIZE)+1); // Corrects the inversion at the cost of every offPoxY = 0 being invalid (should be fixed)

    *index = (offsetX % CHUNK_SIZE) + (abs(offsetY) % CHUNK_SIZE) * CHUNK_SIZE;
}

void chunk_begin(){
    // Load all chunks around the car
    loadChunk(-1,-1);
    loadChunk(-1, 0);
    loadChunk(-1, 1);
    loadChunk(0,-1);
    loadChunk(0, 0);
    loadChunk(0, 1);
    loadChunk(1,-1);
    loadChunk(1, 0);
    loadChunk(1, 1);
}




#endif //CHUNK_HANDLER_H