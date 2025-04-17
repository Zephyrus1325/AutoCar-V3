#ifndef CHUNK_HANDLER_H
#define CHUNK_HANDLER_H

#include <Arduino.h>
#include "configs.h"
#include "util.h"
#include "files.h"

// In this approach, every bit is a position, that can be either:
// 0: Free Space
// 1: Wall
struct Chunk{
    Vector position;                  // Position of a chunk
    int16_t length;                    // length of the walls of a chunk 
    int16_t precision;                 // Precision used in this chunk
    uint8_t status;                  // Status of a chunk 0 - Unloaded | 1 - loaded | 2 - 
    uint8_t data[CHUNK_STORAGE];     // compacted data of a chunk
};

Chunk chunks[9];    // 9 Chunks will be used, in theory, there will be one for each direction, and the center

/*
    Load a chunk into the chunks array
    x - chunk X position
    y - chunk Y position

*/
void loadChunk(int16_t x, int16_t y){
    // Check if chunk is already loaded
    // (Gotta keep cpu usage low)
    for(int i = 0; i < 9; i++){
        if(chunks[i].position.x == x && chunks[i].position.y == y){
            return; // This chunk is loaded, ignore the loading
        }
    }

    // Find a neat algorithm to opmitmize chunk loadings

    // Poor cpu
}

void transmitChunk(){
    for(int i = 0; i < 9; i++){
        for(uint16_t j = 0; j < CHUNK_SUBDIVISION; j++){
            chunk_data data;
            data.position = chunks[i].position;
            data.subdivision = 69;
            memcpy(data.data, &chunks[i].data[j*CHUNK_SUBDIVISION], CHUNK_RADIO_SIZE);
            transmitData(CHUNK_DATA, &data, sizeof(chunk_data));
        }
    }
}

// Gets  x,y nav coordinates and turn into x,y chunk coordinate
void coordinate2chunk(Vector navPos, Vector* chunkPos){
    chunkPos->x = navPos.x / CHUNK_SIZE; 
    chunkPos->y = navPos.y / CHUNK_SIZE;
}

void chunk_begin(){
    chunks[0].position.x = 1;
    chunks[0].position.y = 2;
    chunks[1].position.x = 3;
    chunks[1].position.y = 4;
    chunks[2].position.x = 5;
    chunks[2].position.y = 6;
    chunks[3].position.x = 7;
    chunks[3].position.y = 8;
    chunks[4].position.x = 9;
    chunks[4].position.y = 10;
    chunks[5].position.x = 11;
    chunks[5].position.y = 12;
    chunks[6].position.x = 13;
    chunks[6].position.y = 14;
    chunks[7].position.x = 15;
    chunks[7].position.y = 16;
    chunks[8].position.x = 17;
    chunks[8].position.y = 18;

    transmitChunk();
}




#endif //CHUNK_HANDLER_H