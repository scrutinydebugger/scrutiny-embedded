//    udp_bridge.h
//        Class that open a UDP port and handle a single client.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___UDP_BRIDGE_H___
#define ___UDP_BRIDGE_H___

#include <system_error>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif


#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define GETSOCKETERRNO() (errno)
#define INVALID_SOCKET (-1)

typedef int SOCKET;
typedef sockaddr SOCKADDR;
#endif

#include "abstract_comm_channel.h"

class UdpBridge : public AbstractCommChannel
{
public:
    UdpBridge(uint16_t port);

    static void global_init();
    static void global_close();

    void send(const uint8_t* buffer, int len, int flags = 0);
    int receive(uint8_t* buffer, int len, int flags = 0);
    
    virtual void start();
    virtual void stop();
    virtual void send(const uint8_t* buffer, int len) {send(buffer, len, 0);}
    virtual int receive(uint8_t* buffer, int len) {return receive(buffer, len, 0);}
    
    void set_nonblocking();
    static void throw_system_error(const char* msg);

private:
    uint16_t m_port;
    SOCKET m_sock;                // SOCKET = int for linux, SOCKET for windows
    SOCKADDR m_last_packet_addr;  // SOCKADDR = sockaddr for linux, SOCKADDR for windows

#if defined(_WIN32)
    static WSAData wsa_data;
#endif

};


#endif  // ___UDP_BRIDGE_H___