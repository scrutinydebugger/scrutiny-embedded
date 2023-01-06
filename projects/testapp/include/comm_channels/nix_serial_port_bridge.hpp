//    nix_serial_port_bridge.hpp
//        Class that make a *Nix Serial port available as a communication channel for the Testapp
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___NIX_SERIAL_PORT_BRIDGE_H___
#define ___NIX_SERIAL_PORT_BRIDGE_H___

#include "scrutiny_setup.hpp"

#if SCRUTINY_BUILD_WINDOWS
#error "This file cannot be compiled under Windows"
#endif

#include "abstract_comm_channel.hpp"

#include <string>
#include <cstdint>
#include <cstdlib>
#include "abstract_comm_channel.hpp"

class NixSerialPortBridge : public AbstractCommChannel
{
public:
    NixSerialPortBridge(const std::string &port_name, uint32_t baudrate);

    virtual void stop();
    virtual void start();
    virtual int receive(uint8_t *buffer, int len);
    virtual void send(const uint8_t *buffer, int len);

    static void throw_system_error(const std::string &msg);

private:
    std::string m_port_name;
    uint32_t m_baudrate;
    int m_fd;
};

#endif // ___NIX_SERIAL_PORT_BRIDGE_H___