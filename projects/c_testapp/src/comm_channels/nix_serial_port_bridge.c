//    nix_serial_port_bridge.c
//        Serial interface fopr the C testapp. Used to talk with the server
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_cwrapper.h"

#if SCRUTINY_BUILD_WINDOWS
#error "This file cannot be compiled under Windows"
#endif

#include "nix_serial_port_bridge.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define ERR_RETURN(msg) {fprintf(stderr, "%s\n", msg); return COMM_CHANNEL_STATUS_error;}
#define RETURN_IF_NOT_SUCCESS(status) {if (status != COMM_CHANNEL_STATUS_success){return status;}}

comm_channel_status_e nix_serial_port_init(nix_serial_port_t *serial_port, char *const port_name, uint32_t const baudrate)
{
    serial_port->m_port_name = port_name;
    serial_port->m_baudrate = baudrate;
    return COMM_CHANNEL_STATUS_success;
}


comm_channel_status_e nix_serial_port_start(nix_serial_port_t *serial_port)
{
    serial_port->m_fd = open(serial_port->m_port_name, O_RDWR);
    if (serial_port->m_fd < 0){
        fprintf(stderr, "Cannot open port %s. err=%d", serial_port->m_port_name, errno);
        return COMM_CHANNEL_STATUS_error;
    }

    struct termios tty;
    if (tcgetattr(serial_port->m_fd, &tty) != 0)
    {
        nix_serial_port_stop(serial_port);
        ERR_RETURN("Cannot open port. tcgetattr failed");
    }

    tty.c_cflag &= ~PARENB;                                                      // No Parity
    tty.c_cflag &= ~CSTOPB;                                                      // One stop bit
    tty.c_cflag &= ~CSIZE;                                                       // Clear actual bit configuration
    tty.c_cflag |= CS8;                                                          // 8 bits
    tty.c_cflag &= ~CRTSCTS;                                                     // Disable RTS/CTS
    tty.c_cflag |= CREAD | CLOCAL;                                               // Ignore modem control message
    tty.c_lflag &= ~ICANON;                                                      // Disable canonical mode to read raw binary data
    tty.c_lflag &= ~ECHO;                                                        // Disable echo
    tty.c_lflag &= ~ECHOE;                                                       // Disable erasure
    tty.c_lflag &= ~ECHONL;                                                      // Disable new-line echo
    tty.c_lflag &= ~ISIG;                                                        // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);                                      // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable special char
    tty.c_oflag &= ~OPOST;                                                       // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR;                                                       // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 0; // Non-blocking
    tty.c_cc[VMIN] = 0;

    speed_t baudrate = 0;

    switch (serial_port->m_baudrate)
    {
    case 50:
        baudrate = B50;
        break;
    case 75:
        baudrate = B75;
        break;
    case 110:
        baudrate = B110;
        break;
    case 134:
        baudrate = B134;
        break;
    case 150:
        baudrate = B150;
        break;
    case 200:
        baudrate = B200;
        break;
    case 300:
        baudrate = B300;
        break;
    case 600:
        baudrate = B600;
        break;
    case 1200:
        baudrate = B1200;
        break;
    case 1800:
        baudrate = B1800;
        break;
    case 2400:
        baudrate = B2400;
        break;
    case 4800:
        baudrate = B4800;
        break;
    case 9600:
        baudrate = B9600;
        break;
    case 19200:
        baudrate = B19200;
        break;
    case 38400:
        baudrate = B38400;
        break;
    case 57600:
        baudrate = B57600;
        break;
    case 115200:
        baudrate = B115200;
        break;
    case 230400:
        baudrate = B230400;
        break;
    default:
        nix_serial_port_stop(serial_port);
        ERR_RETURN("Unsupported baudrate");
    }

    if (cfsetispeed(&tty, baudrate) != 0)
    {
        nix_serial_port_stop(serial_port);
        ERR_RETURN("Cannot open port. cfsetispeed failed");
    }

    if (cfsetospeed(&tty, baudrate) != 0)
    {
        nix_serial_port_stop(serial_port);
        ERR_RETURN("Cannot open port. ifcfsetospeed failed");
    }

    if (tcsetattr(serial_port->m_fd, TCSANOW, &tty) != 0)
    {
        nix_serial_port_stop(serial_port);
        ERR_RETURN("Cannot open port. tcsetattr failed");
    }

    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e nix_serial_port_stop(nix_serial_port_t *serial_port)
{
    if (serial_port->m_fd > 0)
    {
        close(serial_port->m_fd);
    }
    serial_port->m_fd = -1;
    return COMM_CHANNEL_STATUS_success;
}


comm_channel_status_e nix_serial_port_receive(nix_serial_port_t *serial_port, uint8_t *buffer, int len, int *ret)
{
    if (serial_port->m_fd < 0){
        return COMM_CHANNEL_STATUS_error;
    }

    *ret = read(serial_port->m_fd, buffer, len);
    if (*ret == -1){
        return COMM_CHANNEL_STATUS_error;
    }

    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e nix_serial_port_send(nix_serial_port_t *serial_port, uint8_t const *buffer, int len)
{
    if (serial_port->m_fd < 0){
        return COMM_CHANNEL_STATUS_error;
    }

    if (write(serial_port->m_fd, buffer, len) == -1){
        return COMM_CHANNEL_STATUS_error;
    }

    return COMM_CHANNEL_STATUS_success;
}
