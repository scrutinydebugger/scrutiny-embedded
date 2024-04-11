#include <stdio.h>
#include <Arduino.h>
#include "scrutiny.hpp"

uint8_t scrutiny_rx_buffer[64];
uint8_t scrutiny_tx_buffer[128];

volatile bool blink2 = true;

scrutiny::Config config;
scrutiny::MainHandler scrutiny_main;
    

void setup() {
    config.set_buffers(
        scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer),     // Receive
        scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer)      // Transmit 
        );
    scrutiny_main.init(&config);
    Serial.begin(115200);
    delay(2000);
    Serial.println("Hello, world!");
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    static uint32_t last_timestamp = micros();
    uint32_t timestamp = micros();
    
    static volatile bool blink = true;
    if (blink && blink2)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }

    scrutiny_main.process(timestamp - last_timestamp);
    last_timestamp = timestamp;
}