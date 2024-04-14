#include <stdio.h>
#include <Arduino.h>
#include "scrutiny.hpp"

uint8_t scrutiny_rx_buffer[64];
uint8_t scrutiny_tx_buffer[128];

volatile bool blink2 = true;

scrutiny::Config config;
scrutiny::MainHandler scrutiny_main;

void update_scrutiny_main(){
    static uint32_t last_timestamp = micros();
    uint32_t timestamp = micros();
    uint8_t buffer[16];
    
    // Connect the streams
    uint16_t const nrx{ static_cast<uint16_t>(Serial.readBytes(buffer, sizeof(buffer))) };
    if (nrx > 0){
        scrutiny_main.comm()->receive_data(buffer, nrx);  
    }
    
    uint16_t const ntx{ scrutiny_main.comm()->pop_data(buffer, sizeof(buffer)) };
    Serial.write(buffer, ntx);

    scrutiny_main.process((timestamp - last_timestamp) * 10);
    last_timestamp = timestamp;
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Setup Scrutiny
    config.set_buffers(
        scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer),     // Receive
        scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer)      // Transmit 
        );
    scrutiny_main.init(&config);
}

constexpr int HIGH_LOW[2] {HIGH, LOW};
void loop() {
    static volatile bool blink = true;
    static uint32_t last_timestamp = micros();
    
    uint8_t state = 0;
    uint32_t timestamp = micros();

    if (blink && blink2)
    {
        if (timestamp - last_timestamp > 200000){
            state = (state+1)&0xFE;
            digitalWrite(LED_BUILTIN, HIGH_LOW[state]);
            last_timestamp = timestamp;
        }
    }

    update_scrutiny_main();
}