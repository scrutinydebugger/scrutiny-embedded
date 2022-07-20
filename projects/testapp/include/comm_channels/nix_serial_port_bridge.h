#ifndef ___NIX_SERIAL_PORT_BRIDGE_H___
#define ___NIX_SERIAL_PORT_BRIDGE_H___

#if defined(_WIN32)
#error "This file cannot be compiled under Windows"
#endif

#include <string>
#include "abstract_comm_channel.h"


class NixSerialPortBridge : public AbstractCommChannel
{
public:
    NixSerialPortBridge(std::string port_name, uint32_t baudrate);
    void start();
    void stop();

    int receive(uint8_t* buffer, int len);
    void send(const uint8_t* buffer, int len);
    static void throw_system_error(const char* msg);

private:
    std::string m_port_name;
    uint32_t m_baudrate;
};


#endif // ___NIX_SERIAL_PORT_BRIDGE_H___