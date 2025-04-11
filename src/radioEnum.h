#include <Arduino.h>
#include "util.h"

struct packet{
    uint8_t message_type;
    uint8_t data[31];
};

enum message_type{
    LOG_MESSAGE = 0x00,         // Sender: CarV3
    
    INFO_NAV = 0x10,            // Sender: CarV3           
    INFO_RADIO = 0x11,          // Sender: CarV3
    INFO_LIDAR = 0x12,          // Sender: CarV3
    INFO_GPS = 0x13,            // Sender: CarV3
    INFO_MOTORLEFT = 0x14,      // Sender: CarV3   
    INFO_MOTORRIGHT = 0x15,     // Sender: CarV3       
    INFO_CHUNK = 0x16,          // Sender: CarV3

    CONFIG_NAV = 0x20,          // Sender: Base    
    CONFIG_RADIO = 0x21,        // Sender: Base        
    CONFIG_LIDAR = 0x22,        // Sender: Base        
    CONFIG_GPS  = 0x23,         // Sender: Base    
    CONFIG_MOTORLEFT = 0x24,    // Sender: Base            
    CONFIG_MOTORRIGHT = 0x25,   // Sender: Base            
    CONFIG_CHUNK = 0x26,        // Sender: Base        

    CHUNK_METADATA = 0x40,      // Sender: CarV3
    CHUNK_DATA = 0x41,          // Sender: CarV3

    COMMAND = 0xFF              // Sender: Both
};

enum command_type{
    REQUEST_DATA = 0x10,        // Sender: Both            
    REQUEST_SEND = 0x11,        // Sender: Both    
    REQUEST_ACKNOLEDGE = 0x12,  // Sender: Both        
    REQUEST_PACKET = 0x13,      // Sender: Both    
    ACKNOLEDGE = 0xAA,          // Sender: Both

    END_OF_PACKET = 0xFF        // Sender: Both
};

struct nav_data{
    Vector position;
    float speed;
};

struct radio_info{
    uint8_t data_rate;
    uint8_t PALevel;
};

struct lidar_info{
    uint8_t rpm;
    uint8_t status;
};

struct motor_info{
    float rpm;
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
    uint8_t data[CHUNK_STORAGE];
};

struct command{
    uint8_t command_type;
    uint8_t parameters[31];
};





