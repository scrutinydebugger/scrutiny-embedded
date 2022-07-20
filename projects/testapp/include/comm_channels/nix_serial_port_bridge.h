#ifndef ___NIX_SERIAL_PORT_BRIDGE_H___
#define ___NIX_SERIAL_PORT_BRIDGE_H___

#if defined(_WIN32)
#error "This file cannot be compiled under Windows"
#endif

#include "abstract_comm_channel.h"

#include <string>
#include <cstdint>
#include <cstdlib>
#include "abstract_comm_channel.h"  

class NixSerialPortBridge : public AbstractCommChannel
{
public:
    NixSerialPortBridge(const std::string& port_name, uint32_t baudrate);

    virtual void stop();
    virtual void start();
    virtual int receive(uint8_t* buffer, size_t len);
    virtual void send(const uint8_t* buffer, size_t len);

    static void throw_system_error(const std::string &msg);

private:
    std::string m_port_name;
    uint32_t m_baudrate;
    int m_fd;
};


#endif // ___NIX_SERIAL_PORT_BRIDGE_H___