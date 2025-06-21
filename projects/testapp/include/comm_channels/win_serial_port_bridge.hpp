//    win_serial_port_bridge.hpp
//        Class that make a Windows Serial port available as a communication channel for the
//        Testapp
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___WIN_SERIAL_PORT_BRIDGE_H___
#define ___WIN_SERIAL_PORT_BRIDGE_H___

#if !defined(_WIN32)
#error "File designed for windows"
#endif

#include "abstract_comm_channel.hpp"

#include <stdint.h>
#include <string>
#include <windows.h>

class WinSerialPortBridge : public AbstractCommChannel
{
  public:
    WinSerialPortBridge(const std::string &port_name, uint32_t baudrate);

    virtual void stop();
    virtual void start();
    virtual int receive(uint8_t *buffer, int len);
    virtual void send(uint8_t const *buffer, int len);

    static void throw_system_error(const std::string &msg);

  private:
    std::string m_port_name;
    uint32_t m_baudrate;
    HANDLE m_serial_handle;
};

#endif // ___WIN_SERIAL_PORT_BRIDGE_H___