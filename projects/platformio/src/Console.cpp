#include "Console.hpp"
#include <SSD1306AsciiSpi.h>

extern SSD1306AsciiSpi display;

static FILE uartout = { 0 };
static int console_putchar(char c, FILE *stream)
{
    return display.write(c);
}

void setup_console()
{
    fdev_setup_stream(&uartout, console_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &uartout;
}
