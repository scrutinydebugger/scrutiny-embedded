
#include <SPI.h>
#include <SSD1306AsciiSpi.h>
#include "Console.hpp"

SSD1306AsciiSpi display;

#include <scrutiny.h>
scrutiny::MainHandler scrutiny_handler;

volatile uint32_t last_call_us = 0;

uint8_t scrutiny_rx_buffer[32];
uint8_t scrutiny_tx_buffer[48];

void setup()
{
#define OLED_DC 9
#define OLED_CS 10
#define OLED_RESET 8
    display.begin(&Adafruit128x64, OLED_CS, OLED_DC, OLED_RESET);
    display.setFont(Adafruit5x7);
    display.setScrollMode(SCROLL_MODE_AUTO);

    setup_console();
    printf("Console rdy\n");

    scrutiny::Config config;
    config.set_buffers(scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer), scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer));
    config.max_bitrate = 100000;
    config.display_name = "Arduino";

    scrutiny_handler.init(&config);
    printf("Scrutiny rdy\n");

    printf("%p\n", &last_call_us);

    Serial.begin(115200);
}

void loop()
{
    uint32_t current_us = micros();
    uint32_t timestep_us = current_us - last_call_us;

    int16_t c = Serial.read();
    if (c != -1)
    {
        uint8_t uc = static_cast<uint8_t>(c);
        scrutiny_handler.receive_data(&uc, 1);
    }

    scrutiny_handler.process(timestep_us * 10);

    uint8_t buffer[16];
    uint32_t data_to_send = scrutiny_handler.data_to_send();
    data_to_send = min(data_to_send, static_cast<uint32_t>(sizeof(buffer)));
    if (data_to_send > 0)
    {
        scrutiny_handler.pop_data(buffer, data_to_send);
        Serial.write(buffer, data_to_send);
    }

    last_call_us = current_us;
}
