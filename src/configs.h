// Core Config
#define MAIN_CORE 1
#define AUX_CORE 0

// Chunks Config
#define CHUNK_SIZE 512 // length of a chunk
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE // area used by a chunk
#define CHUNK_STORAGE CHUNK_AREA / 8 // storage used per chunk
#define CHUNK_SUBDIVISION 64         // How to subdivide a chunk for faster sending (Should be power of 2)
#define CHUNK_RADIO_SIZE CHUNK_STORAGE / CHUNK_SUBDIVISION // 512 bytes per subdivision

// Radio Config
#define RADIO_QUEUE_SIZE 50     // Number of data quests
#define RECEIVE_BUFFER_SIZE CHUNK_RADIO_SIZE // Size of radio RX buffer (bytes)
#define MAX_LOG_MESSAGE 64

// Motor Config
#define MOTOR_UPDATE_TIME 10      // Time between each motor update
#define MOTOR_WHEEL_RADIUS 15.6f  


// Pins Config
// Motor Pins
#define LEFT_MOTOR_A    2
#define LEFT_MOTOR_B    3
#define LEFT_MOTOR_PWM  4
#define LEFT_MOTOR_ENCODER 5
#define RIGHT_MOTOR_A    6
#define RIGHT_MOTOR_B    7
#define RIGHT_MOTOR_PWM  8
#define RIGHT_MOTOR_ENCODER 9

// General Pin Config
#define SD_PIN 15
#define RF24_CE_PIN 5
#define RF24_CSN_PIN 2
#define LIDAR_PIN 25



