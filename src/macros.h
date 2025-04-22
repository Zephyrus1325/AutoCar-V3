#ifndef MACROS_H
#define MACROS_H

#include <Arduino.h>


// Enable debug for things such as data logging
#define DEBUG

#ifdef DEBUG

#include "radio.h"
#include "radioEnum.h"

// Used for data logging in many cases
void log(const String text){
    log_message msg;
    Serial.print(text);
    strncpy(msg.text, text.c_str(), text.length()+1);
    transmitData(LOG_MESSAGE, &msg, sizeof(log_message));
}

void error(String text){
    log_message msg;
    String errorText = "ERROR: " + text;
    Serial.print(errorText);
    msg.setText(errorText.c_str()); 
    transmitData(LOG_MESSAGE, &msg, sizeof(log_message));
}

#else // DEBUG
    void log(const char text[]){}
    void error(const char text[]){}
#endif //DEBUG


#endif //MACROS_H