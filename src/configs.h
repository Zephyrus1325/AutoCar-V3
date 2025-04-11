#define CHUNK_SIZE 512 // length of a chunk
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE // area used by a chunk
#define CHUNK_STORAGE CHUNK_AREA / 8 // storage used per chunk

#define SEND_BUFFER_SIZE 1152 // Size of radio TX buffer (in packets)
#define RECEIVE_BUFFER_SIZE 100 // Size of radio RX buffer (in packets)
#define MAX_TX_ATTEMPTS 100     // Max attempts before timeout

#define SD_PIN 5
#define RF24_CE_PIN 48
#define RF24_CSN_PIN 49
#define LIDAR_PIN 25