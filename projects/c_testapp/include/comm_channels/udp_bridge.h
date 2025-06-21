//    udp_bridge.h
//        UDP interface fopr the C testapp. Used to talk with the server
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___UDP_BRIDGE_H___
#define ___UDP_BRIDGE_H___

#include "abstract_comm_channel.h"

#if SCRUTINY_BUILD_WINDOWS
#include <winsock2.h>
#else
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#if SCRUTINY_BUILD_WINDOWS
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define GETSOCKETERRNO() (errno)
#define INVALID_SOCKET (-1)

typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
#endif

typedef struct
{
    uint16_t m_port;
    SOCKET m_sock;               // SOCKET = int for linux, SOCKET for windows
    SOCKADDR m_last_packet_addr; // SOCKADDR = sockaddr for linux, SOCKADDR for windows

#if SCRUTINY_BUILD_WINDOWS
    struct WSAData wsa_data;
#endif
} udp_bridge_t;

comm_channel_status_e udp_bridge_global_init();
comm_channel_status_e udp_bridge_global_close();

void udp_bridge_init(udp_bridge_t *bridge, uint16_t port);
comm_channel_status_e _udp_bridge_send(udp_bridge_t *bridge, uint8_t const *buffer, int len, int flags);
comm_channel_status_e _udp_bridge_receive(udp_bridge_t *bridge, uint8_t *buffer, int len, int flags, int *ret);

comm_channel_status_e udp_bridge_start(udp_bridge_t *bridge);
comm_channel_status_e udp_bridge_stop(udp_bridge_t *bridge);
comm_channel_status_e udp_bridge_send(udp_bridge_t *bridge, uint8_t const *buffer, int len);
comm_channel_status_e udp_bridge_receive(udp_bridge_t *bridge, uint8_t *buffer, int len, int *ret);

comm_channel_status_e udp_bridge_set_nonblocking(udp_bridge_t *bridge);

#endif // ___UDP_BRIDGE_H___
