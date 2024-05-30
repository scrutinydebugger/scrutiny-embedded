//    udp_bridge.c
//        UDP interface fopr the C testapp. Used to talk with the server
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_cwrapper.h"
#include "udp_bridge.h"
#include "stdlib.h"
#include "stdio.h"

#define ERR_RETURN(msg) {fprintf(stderr, "%s\n", msg); return COMM_CHANNEL_STATUS_error;}
#define RETURN_IF_NOT_SUCCESS(status) {if (status != COMM_CHANNEL_STATUS_success){return status;}}

#if SCRUTINY_BUILD_WINDOWS
#include <windows.h>
WSAData UdpBridge::wsa_data;
#else
#include <fcntl.h> // For non-blocking
#endif

comm_channel_status_e udp_bridge_global_init()
{
#if SCRUTINY_BUILD_WINDOWS
    int ret = WSAStartup(MAKEWORD(2, 2), &UdpBridge::wsa_data); // Assume one UDP bridge will be running globally. We don't need more really
    if (ret != 0)
    {
        ERR_RETURN("WSAStartup Failed");
    }
#endif
    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e udp_bridge_global_close()
{
#if SCRUTINY_BUILD_WINDOWS
    WSACleanup();
#endif
return COMM_CHANNEL_STATUS_success;
}


void udp_bridge_init(udp_bridge_t* bridge, uint16_t port)
{
    bridge->m_port=port;
    bridge->m_sock=INVALID_SOCKET;
}

comm_channel_status_e udp_bridge_start(udp_bridge_t* bridge)
{
    int ret;
    comm_channel_status_e status;

    bridge->m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (!ISVALIDSOCKET(bridge->m_sock))
    {
        ERR_RETURN("Error opening socket");
    }

    status = udp_bridge_set_nonblocking(bridge); // Set m_sock non blocking
    RETURN_IF_NOT_SUCCESS(status);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(bridge->m_port);

    ret = bind(bridge->m_sock, (SOCKADDR*)&addr, sizeof(addr)); // SOCKADDR = sockaddr for linux

    if (ret < 0)
    {
        ERR_RETURN("Bind failed");
    }

    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e udp_bridge_set_nonblocking(udp_bridge_t* bridge)
{
#if SCRUTINY_BUILD_WINDOWS
    unsigned long mode = 1;
    if (ioctlsocket(bridge->m_sock, FIONBIO, &mode) != NO_ERROR)
    {
        ERR_RETURN("ioctlsocket failed");
    }
#else
    int flags = fcntl(bridge->m_sock, F_GETFL, 0);
    if (flags == -1)
    {
        ERR_RETURN("F_GETFL failed");
    }

    flags |= O_NONBLOCK;
    if (fcntl(bridge->m_sock, F_SETFL, flags) != 0)
    {
        ERR_RETURN("F_SETFL failed");
    }
#endif
    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e udp_bridge_stop(udp_bridge_t* bridge)
{
    if (ISVALIDSOCKET(bridge->m_sock))
    {
        CLOSESOCKET(bridge->m_sock); // CLOSESOCKET() is a cross-platform macro defined in udp_bridge.h
    }
    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e _udp_bridge_receive(udp_bridge_t* bridge, uint8_t *buffer, int len, int flags, int *ret)
{
#if SCRUTINY_BUILD_WINDOWS
    int size = sizeof(bridge->m_last_packet_addr);
#else
    socklen_t size = sizeof(bridge->m_last_packet_addr);
#endif
    *ret = recvfrom(bridge->m_sock, (char *)buffer, len, flags, &bridge->m_last_packet_addr, &size);

    if (*ret < 0)
    {
        int errorcode = GETSOCKETERRNO(); // GETSOCKETERRNO() is a cross-platform macro defined in udp_bridge.h
#if SCRUTINY_BUILD_WINDOWS
        if (errorcode == WSAEWOULDBLOCK)
#else
        if (errorcode == EWOULDBLOCK || errorcode == EAGAIN) // Need to check both as per unix manual
#endif
        {
            *ret = 0;
        }
        else
        {
            ERR_RETURN("recvfrom failed");
        }
    }

    return COMM_CHANNEL_STATUS_success;
}


comm_channel_status_e _udp_bridge_send(udp_bridge_t* bridge, const uint8_t *buffer, int len, int flags)
{
    int ret = sendto(bridge->m_sock, (const char *)buffer, len, flags, &bridge->m_last_packet_addr, sizeof(bridge->m_last_packet_addr));

    if (ret < 0)
    {
        ERR_RETURN("sendto failed");
    }
    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e udp_bridge_send(udp_bridge_t* bridge, const uint8_t *buffer, int len) 
{ 
    return _udp_bridge_send(bridge, buffer, len, 0); 
}

comm_channel_status_e udp_bridge_receive(udp_bridge_t* bridge, uint8_t *buffer, int len, int* ret) 
{ 
    return _udp_bridge_receive(bridge, buffer, len, 0, ret); 
}
