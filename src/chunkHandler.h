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
    uint8_t status;                  // Status of a chunk 0 - Unloaded | 1 - loaded | 2 - loading
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


// Gets  x,y nav coordinates and turn into x,y chunk coordinate
void coordinate2chunk(Vector navPos, Vector* chunkPos){
    chunkPos->x = navPos.x / CHUNK_SIZE; 
    chunkPos->y = navPos.y / CHUNK_SIZE;
}




#endif //CHUNK_HANDLER_H