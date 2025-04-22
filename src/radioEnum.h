#ifndef RADIO_ENUM_H
#define RADIO_ENUM_H
#include <Arduino.h>
#include "util.h"
#include "configs.h"
#include "string.h"

QueueHandle_t radioQueue;

enum message_type{
    LOG_MESSAGE = 0x00,         // Sender: CarV3
    
    INFO_NAV = 0x10,            // Sender: CarV3           
    INFO_RADIO = 0x11,          // Sender: CarV3
    INFO_LIDAR = 0x12,          // Sender: CarV3
    INFO_GPS = 0x13,            // Sender: CarV3
    INFO_MOTOR = 0x14,          // Sender: CarV3      
    INFO_CHUNK = 0x16,          // Sender: CarV3

    CONFIG_NAV = 0x20,          // Sender: Base    
    CONFIG_RADIO = 0x21,        // Sender: Base        
    CONFIG_LIDAR = 0x22,        // Sender: Base        
    CONFIG_GPS  = 0x23,         // Sender: Base    
    CONFIG_MOTOR = 0x24,        // Sender: Base                    
    CONFIG_CHUNK = 0x26,        // Sender: Base        

    CHUNK_METADATA = 0x40,      // Sender: CarV3
    CHUNK_DATA = 0x41,          // Sender: CarV3

    COMMAND = 0xFF              // Sender: Both
};

// Main method of moving queue data around

struct radioQueueData{
    uint8_t messageType;    // Message type
    uint8_t data[100];      // Pointer to the data to be sent
};


struct log_message {
    uint8_t length;
    char text[MAX_LOG_MESSAGE];
    void setText(const char* str) {
        strncpy(text, str, MAX_LOG_MESSAGE);
        text[MAX_LOG_MESSAGE - 1] = '\0'; // segurança
        length = strlen(text);
    }
};

struct nav_data{
    Vector position;
    float speed;
    float heading;
};

struct radio_info{
    uint8_t data_rate;
    uint8_t PALevel;
};

struct lidar_info{
    uint8_t rpm;
    uint8_t status;
};

struct gps_info{
    // I dont know it what to put in here  
};

struct motor_info{
    uint8_t side;
    float rpm;
    float speed;
    float setpoint;
    int16_t throttle;
    float kp;
    float ki;
    float kd;
};

struct chunk_metadata{
    Vector position;
    int16_t length;                    // length of the walls of a chunk 
    int16_t precision;                 // Precision used in this chunk
};

struct chunk_data{
    Vector position;                  // Chunk position
    uint16_t subdivision;             // Subdivion id
    uint8_t data[CHUNK_RADIO_SIZE];   // data inside subdivision
};

struct command{
    uint8_t command_type;
    uint8_t parameters[31];
};

enum command_type{
    PING = 0,

    NAVIGATION_CHANGE_DESTINATION,
    NAVIGATION_CHANGE_MODE,

    RADIO_CHANGE_PA,
    RADIO_CHANGE_CHANNEL,

    LIDAR_CHANGE_SPEED,

    MOTORL_CHANGE_MODE,
    MOTORL_CHANGE_KP,
    MOTORL_CHANGE_KI,
    MOTORL_CHANGE_KD,
    MOTORL_CHANGE_SETPOINT,
    
    MOTORR_CHANGE_MODE,
    MOTORR_CHANGE_KP,
    MOTORR_CHANGE_KI,
    MOTORR_CHANGE_KD,
    MOTORR_CHANGE_SETPOINT,
};

#endif // RADIO_ENUM_H



