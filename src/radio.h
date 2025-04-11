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




#endif //RADIO_H