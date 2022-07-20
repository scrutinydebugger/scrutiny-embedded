#if defined(_WIN32)
#error "This file cannot be compiled under Windows"
#endif

#include "nix_serial_port_bridge.h"
#include <string>

NixSerialPortBridge::NixSerialPortBridge(std::string port_name, uint32_t baudrate) : 
    m_port_name(port_name),
    m_baudrate(baudrate)
{
    
}


void NixSerialPortBridge::stop()
{
    
}