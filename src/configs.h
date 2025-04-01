#define CHUNK_SIZE 512 // length of a chunk
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE // area used by a chunk
#define CHUNK_STORAGE CHUNK_AREA / 8 // storage used per chunk

#define SD_PIN 5
#define RF24_CE_PIN 48
#define RF24_CSN_PIN 49