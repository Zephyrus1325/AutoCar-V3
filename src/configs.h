// Core Config
#define MAIN_CORE 1
#define AUX_CORE 0
#define MIN_DELAY 2 // Minimum delay of each task
// Chunks Config
#define UNIT_SIZE 10   // CM per unit of a chunk
#define CHUNK_SIZE 32 // length of a chunk
#define CHUNK_AREA CHUNK_SIZE * CHUNK_SIZE // area used by a chunk in units
#define CHUNK_STORAGE CHUNK_AREA    // storage used per chunk
#define CHUNK_SUBDIVISION 8         // How to subdivide a chunk for faster sending (Should be power of 2)
#define CHUNK_RADIO_SIZE (CHUNK_STORAGE) / CHUNK_SUBDIVISION // 128 bytes per subdivision
#define CHUNK_SAVE_TIME 10000   // Wait time for a chunk to be saved onto SD card, for redundancy

// Radio Config
#define RADIO_QUEUE_SIZE 15     // Number of data quests
#define CHUNK_QUEUE_SIZE 250     //
#define RADIO_MAX_TRIES 16       // Maximum number of send tries befores giving up (except chunk data)
#define MAX_LOG_MESSAGE 96      // Maximum length of a log message

// Update delay for each type of data
#define DELAY_NAV_DATA 100
#define DELAY_LIDAR_DATA 1000
#define DELAY_GPS_DATA 1000
#define DELAY_MOTOR_DATA 100
#define DELAY_CHUNK_TRANSMIT 2000
#define DELAY_CHUNK_STORE 10000

// SD config
#define SD_QUEUE_SIZE 20        // Number of Read/Writes Queue
// Motor Config
#define MOTOR_UPDATE_TIME 10      // Time between each motor update
#define MOTOR_WHEEL_RADIUS 15.6f    // Radius of wheel in cm

// Sensors Config
#define SENSOR_UPDATE_PERIOD 100 // Update wait time (millis)

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
#define RF24_CE_PIN 4
#define RF24_CSN_PIN 2
#define LIDAR_PIN 13



