#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "configs.h"
#include "pid.h"

#include "radioEnum.h"

class Motor{

    private:
    uint8_t pin_a;
    uint8_t pin_b;
    uint8_t pin_pwm;
    uint8_t pin_encoder; 

    float speed;
    uint64_t lastEncoderUpdate = 0;

    PID pid;


    public:
    Motor(uint8_t a, uint8_t b, uint8_t pwm, uint8_t encoder){
        pin_a = a;
        pin_b = b;
        pin_pwm = pwm;
        pin_encoder = encoder;
    }

    void begin(){
        pinMode(pin_a, OUTPUT);
        pinMode(pin_b, OUTPUT);
        pinMode(pin_pwm, OUTPUT); 
        pinMode(pin_encoder, INPUT);    
        pid.resetIntegral();
        pid.resetLastUpdate();
        pid.setError(0);
        pid.setKd(1);   // TODO: Retrieve Last Config Data From a File
        pid.setKi(1);   // TODO: Retrieve Last Config Data From a File
        pid.setKd(1);   // TODO: Retrieve Last Config Data From a File
    }

    void update(){
        pid.update();
    }

    void updateEncoder(){
        // Insert Speed updater here
        lastEncoderUpdate = micros();
    }

    void setSpeed(int speed){
        if(speed < 0){
            digitalWrite(pin_a, LOW);
            digitalWrite(pin_b, HIGH);
        } else {
            digitalWrite(pin_a, HIGH);
            digitalWrite(pin_b, LOW);
        }
        analogWrite(pin_pwm, constrain(abs(speed),0,255));
    }
};

Motor* leftM;
Motor* rightM;

static void leftEncoder(){
    leftM->updateEncoder();
}

static void rightEncoder(){
    rightM->updateEncoder();
}


void motorTask(void* param){
    Motor leftMotor(LEFT_MOTOR_A, LEFT_MOTOR_B, LEFT_MOTOR_PWM, LEFT_MOTOR_ENCODER);
    Motor rightMotor(RIGHT_MOTOR_A, RIGHT_MOTOR_B, RIGHT_MOTOR_PWM, RIGHT_MOTOR_ENCODER);

    leftM = &leftMotor;
    rightM = &rightMotor;
    
    leftMotor.begin();
    rightMotor.begin();

    attachInterrupt(digitalPinToInterrupt(LEFT_MOTOR_ENCODER), leftEncoder, RISING);
    attachInterrupt(digitalPinToInterrupt(RIGHT_MOTOR_ENCODER), rightEncoder, RISING);

    while(true){
        leftMotor.update();
        rightMotor.update();

        motor_info left_info{0, };


        vTaskDelay(pdMS_TO_TICKS(MOTOR_UPDATE_TIME));
    }
}

#endif // MOTOR_H