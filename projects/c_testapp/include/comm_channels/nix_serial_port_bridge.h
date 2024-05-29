//    nix_serial_port_bridge.h
//        CommChannel extension that make a *Nix Serial port available as a communication channel for the Testapp
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___NIX_SERIAL_PORT_BRIDGE_H___
#define ___NIX_SERIAL_PORT_BRIDGE_H___

#include "scrutiny_cwrapper.h"

#if SCRUTINY_BUILD_WINDOWS
#error "This file cannot be compiled under Windows"
#endif

#include "abstract_comm_channel.h"
#include <stdint.h>

typedef struct
{
    const char *m_port_name;
    uint32_t m_baudrate;
    int m_fd;
} nix_serial_port_t;

comm_channel_status_e nix_serial_port_init(nix_serial_port_t *serial_port, char *const port_name, uint32_t const baudrate);
comm_channel_status_e nix_serial_port_stop(nix_serial_port_t *serial_port);
comm_channel_status_e nix_serial_port_start(nix_serial_port_t *serial_port);
comm_channel_status_e nix_serial_port_receive(nix_serial_port_t *serial_port, uint8_t *buffer, int len, int *ret);
comm_channel_status_e nix_serial_port_send(nix_serial_port_t *serial_port, uint8_t const *buffer, int len);

#endif // ___NIX_SERIAL_PORT_BRIDGE_H___