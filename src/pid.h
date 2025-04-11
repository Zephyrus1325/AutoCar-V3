#ifndef PID_H
#define PID_H


#include "Arduino.h"

class PID{
        private:
        float kp = 0;
        float ki = 0;
        float kd = 0;
        float integral = 0;
        float setpoint = 0;
        float actualValue = 0;
        float lastValue = 0;
        unsigned long lastUpdateTime = 0;
        float output = 0;
        bool manualError = false;
        float error = 0;
        public:

        void update(){
            if(!manualError){
                error = setpoint - actualValue;
            }
            float PIDderivative = (actualValue - lastValue) * (millis() - lastUpdateTime)/1000.0f;
            integral += error * (float)(millis() - lastUpdateTime)/1000.0f;
            float proportional =  kp * error;
            float integralout =  ki * integral;
            float derivative =  kd * PIDderivative;

            output = proportional + integralout + derivative;
            
            lastUpdateTime = millis();
            lastValue = actualValue;
        }

        float getOutput(){
            return output;
        }

        float getSetpoint(){
            return setpoint;
        }

        float getActualValue(){
            return actualValue;
        }

        float getKp(){
            return kp;
        }

        float getKi(){
            return ki;
        }

        float getKd(){
            return kd;
        }

        float getError(){
            return error;
        }

        void resetIntegral(){
            integral = 0;
        }

        void resetLastUpdate(){
            lastUpdateTime = millis();
        }

        void setKp(float value){
            kp = value;
        }

        void setKi(float value){
            ki = value;
        }

        void setKd(float value){
            kd = value;
        }

        void setError(float newError){
            error = newError;
        }

        void setManualError(bool manual){
            manualError = manual;
        }

        void setActualValue(float value){
            actualValue = value;
        }

        void setSetpoint(float value){
            setpoint = value;
        }
    };


#endif // PID_H