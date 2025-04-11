#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "RF24.h"
#include <SPI.h>
#include "macros.h"

#include "radioEnum.h"

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);

uint8_t address[][6] = { "CarV3", "Base " };

packet send_buffer[SEND_BUFFER_SIZE];
packet receive_buffer[RECEIVE_BUFFER_SIZE];

uint16_t send_index = 0;
uint16_t receive_index = 0;
bool sending = false; // 0 - Listening | 1 - Sending 
uint8_t send_counter = 0; // Data to be sent
/*
    Protocolo:
    CarV3 -> Quero mandar dados do tipo XXXX, com N Paginas, tudo certo?
    Base  -> Dados do tipo XXXXX com N paginas, pode mandar!
    CarV3 -> Envia os dados com Pagina N
    CarV3 -> Mandei tudo, recebeu?
    Base  -> (Recebeu tudo) Recebi N paginas, tudo certo!
    Base  -> (Não recebeu tudo) Recebi N paginas, se prepara para enviar as seguintes paginas
    Base  -> Manda todos os pacotes não recebidos
    CarV3 -> Recebi todos, vou te mandar então
    CarV3 -> Manda os dados faltantes
    CarV3 -> Mandei tudo, tudo certo
    Base  -> Tudo certo
    CarV3 -> Fim da transmissão
*/

void radioTask(void* param){

    if (!radio.begin()) {
        error("radio hardware is not responding!!");
        while (1) {}  // hold in infinite loop
    }
    
    radio.setPALevel(RF24_PA_LOW);  
    radio.setPayloadSize(sizeof(packet));  
    radio.openWritingPipe(address[0]);  
    radio.openReadingPipe(1, address[1]);
    radio.startListening();

    while(true){
        esp_task_wdt_reset();

        uint16_t tx_attempts = 0
        // TX part
        while(send_counter > 0 && tx_attempts < MAX_TX_ATTEMPTS){
            esp_task_wdt_reset();
            bool report = radio.write(send_buffer[send_index], sizeof(packet));  // transmit & save the report
            if(report){
                send_counter--;
                send_index = ++send_index >= SEND_BUFFER_SIZE ? 0 : send_index;
            } else {
                tx_attempts++;
                if(tx_attempts == MAX_TX_ATTEMPTS){
                    error("Max radio attempts Reached, timeout");
                }
            }
        }

        // RX part
        if(radio.available(&pipe)){
            uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
            radio.read(receive_buffer[receive_index], bytes);             // fetch payload from FIFO
            receive_index = ++receive_index >= RECEIVE_BUFFER_SIZE ? 0 : receive_index;
        }
    }
}

/*

<<<<<<< HEAD
=======
void setup() {


  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  

  // additional setup specific to the node's role
  if (role) {
    radio.stopListening();  // put radio in TX mode
  } else {
    radio.startListening();  // put radio in RX mode
  }

  // For debugging info
  // printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  // radio.printPrettyDetails(); // (larger) function that prints human readable data

}  // setup

void loop() {

  if (role) {
    // This device is a TX node

    unsigned long start_timer = micros();                // start the timer
    bool report = radio.write(&payload, sizeof(float));  // transmit & save the report
    unsigned long end_timer = micros();                  // end the timer

    if (report) {
      Serial.print(F("Transmission successful! "));  // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);  // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.println(payload);  // print payload sent
      payload += 0.01;          // increment float payload
    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }

    // to make this example readable in the serial monitor
    delay(1000);  // slow transmissions down by 1 second

  } else {
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {              // is there a payload? get the pipe number that received it
      uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
      radio.read(&payload, bytes);             // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);  // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);  // print the pipe number
      Serial.print(F(": "));
      Serial.println(payload);  // print the payload's value
    }
  }  // role

  if (Serial.available()) {
    // change the role via the serial monitor

    char c = toupper(Serial.read());
    if (c == 'T' && !role) {
      // Become the TX node

      role = true;
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      radio.stopListening();

    } else if (c == 'R' && role) {
      // Become the RX node

      role = false;
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      radio.startListening();
    }
  }

}  // loop
*/
>>>>>>> 3d6493f4fa5116e4e9bf2eb9d4ecbdd00ee102fe

#endif //RADIO_H