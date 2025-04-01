#ifndef MACROS_H
#define MACROS_H

#include <Arduino.h>


// Enable debug for things such as data logging
#define DEBUG



#ifdef DEBUG
// Used for data logging in many cases
#define log(text) Serial.print(text)
#define error(text) Serial.print("ERROR: " text);

#endif //DEBUG


#endif //MACROS_H