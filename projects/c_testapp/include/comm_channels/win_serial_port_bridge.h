//    win_serial_port_bridge.h
//        Serial interface fopr the C testapp. Used to talk with the server
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

#include "abstract_comm_channel.h"

#include <stdint.h>
#include <windows.h>

typedef struct
{
    char const *m_portname;
    uint32_t m_baudrate;
    HANDLE m_serial_handle;
} win_serial_port_t;

comm_channel_status_e win_serial_port_init(win_serial_port_t *serial_port, char *const port_name, uint32_t const baudrate);
comm_channel_status_e win_serial_port_stop(win_serial_port_t *serial_port);
comm_channel_status_e win_serial_port_start(win_serial_port_t *serial_port);
comm_channel_status_e win_serial_port_receive(win_serial_port_t *serial_port, uint8_t *buffer, int len, int *ret);
comm_channel_status_e win_serial_port_send(win_serial_port_t *serial_port, uint8_t const *buffer, int len);

#endif // ___WIN_SERIAL_PORT_BRIDGE_H___
