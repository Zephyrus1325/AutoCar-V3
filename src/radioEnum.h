#include <Arduino.h>
#include "util.h"

struct payload{
    uint8_t message_type;
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
    float 
};

struct radio_info{

};

struct lidar_info{

};

struct motor_info{

};

struct chunk_metadata{

};

struct chunk_data{

};

struct command{

};





