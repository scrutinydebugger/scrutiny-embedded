//    nix_serial_port_bridge.cpp
//        Class that make a *Nix Serial port available as a communication channel for the Testapp
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_setup.hpp"

#if SCRUTINY_BUILD_WINDOWS
#error "This file cannot be compiled under Windows"
#endif

#include "nix_serial_port_bridge.hpp"
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <string>
#include <system_error>

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

NixSerialPortBridge::NixSerialPortBridge(const std::string &port_name, uint32_t baudrate) :
    m_port_name(port_name),
    m_baudrate(baudrate),
    m_fd(-1)
{
}

void NixSerialPortBridge::start()
{
    m_fd = open(m_port_name.c_str(), O_RDWR);
    if (m_fd < 0)
    {
        throw_system_error(std::string("Cannot open port ") + m_port_name);
    }

    struct termios tty;
    if (tcgetattr(m_fd, &tty) != 0)
    {
        stop();
        throw_system_error("Cannot open port. tcgetattr failed");
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
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 0; // Non-blocking
    tty.c_cc[VMIN] = 0;

    speed_t baudrate = 0;

    switch (m_baudrate)
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
        stop();
        throw_system_error("Unsupported baudrate");
    }

    if (cfsetispeed(&tty, baudrate) != 0)
    {
        stop();
        throw_system_error("Cannot open port. cfsetispeed failed");
    }

    if (cfsetospeed(&tty, baudrate) != 0)
    {
        stop();
        throw_system_error("Cannot open port. ifcfsetospeed failed");
    }

    if (tcsetattr(m_fd, TCSANOW, &tty) != 0)
    {
        stop();
        throw_system_error("Cannot open port. tcsetattr failed");
    }
}

void NixSerialPortBridge::stop()
{
    if (m_fd > 0)
    {
        close(m_fd);
    }
    m_fd = -1;
}

int NixSerialPortBridge::receive(uint8_t *buffer, int len)
{
    int n = 0;
    if (m_fd > 0)
    {
        n = read(m_fd, buffer, len);
    }
    return n;
}

void NixSerialPortBridge::send(uint8_t const *buffer, int len)
{
    if (m_fd > 0)
    {
        write(m_fd, buffer, len);
    }
}

void NixSerialPortBridge::throw_system_error(const std::string &msg)
{
    throw std::system_error(errno, std::system_category(), msg.c_str());
}
