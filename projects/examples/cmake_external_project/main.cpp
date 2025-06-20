#include <iostream>
#include <stdint.h>
#include "scrutiny.hpp"

uint8_t rx_buffer[128];
uint8_t tx_buffer[128];

int main(int argc, char *argv[])
{
    scrutiny::Config config;
    scrutiny::MainHandler scrutiny_handler;

    config.set_buffers(rx_buffer, sizeof(rx_buffer), tx_buffer, sizeof(tx_buffer));
    scrutiny_handler.init(&config);

    std::cout << std::endl
              << "-> You made it! Project built and linked to Scrutiny successfully." << std::endl
              << std::endl;
    return 0;
}