#include "udp_bridge.h"
#include <thread>
#include <chrono>

#if defined(_WIN32)
WSAData UdpBridge::wsa_data;
#else
#include <fcntl.h>  // For non-blocking
#endif

UdpBridge::UdpBridge(uint16_t port) :
    m_port(port),
    m_sock(INVALID_SOCKET)
{

}


void UdpBridge::global_init()
{
#if defined(_WIN32)
    int ret = WSAStartup(MAKEWORD(2, 2), &UdpBridge::wsa_data);  // Assume one UDP bridge will be running globally. We don't need more really
    if (ret != 0)
    {
        throw_system_error("WSAStartup Failed");
    }
#endif
}

void UdpBridge::global_close()
{
#if defined(_WIN32)
    WSACleanup();
#endif
}



void UdpBridge::start()
{
    int ret;

    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (!ISVALIDSOCKET(m_sock))
    {
        throw_system_error("Error opening socket");
    }

    set_nonblocking();  // Set m_sock non blocking

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(m_port);

    ret = bind(m_sock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr));   // SOCKADDR = sockaddr for linux

    if (ret < 0)
    {
        throw_system_error("Bind failed");
    }
}

void UdpBridge::set_nonblocking()
{
#if defined(_WIN32)
    unsigned long mode = 1;
    if (ioctlsocket(m_sock, FIONBIO, &mode) != NO_ERROR)
    {
        throw_system_error("ioctlsocket failed");
    }
#else
    int flags = fcntl(m_sock, F_GETFL, 0);
    if (flags == -1) 
    {
        throw_system_error("F_GETFL failed");
    }
    
    flags |= O_NONBLOCK;
    if (fcntl(m_sock, F_SETFL, flags) != 0)
    {
        throw_system_error("F_SETFL failed");
    }
#endif

}

void UdpBridge::throw_system_error(const char* msg)
{
    // GETSOCKETERRNO() is a cross-platform macro defined in udp_bridge.h
    throw std::system_error(GETSOCKETERRNO(), std::system_category(), msg);
}



void UdpBridge::stop()
{
    if (ISVALIDSOCKET(m_sock))
    {
        CLOSESOCKET(m_sock);    // CLOSESOCKET() is a cross-platform macro defined in udp_bridge.h
    }
}

int UdpBridge::receive(uint8_t* buffer, int len, int flags)
{ 
#if defined(_WIN32)
    int size = sizeof(m_last_packet_addr);
#else
    socklen_t size = sizeof(m_last_packet_addr);
#endif
    int ret = recvfrom(m_sock, reinterpret_cast<char*>(buffer), len, flags, &m_last_packet_addr, &size);
    
    if (ret < 0)
    {
        int errorcode = GETSOCKETERRNO();   // GETSOCKETERRNO() is a cross-platform macro defined in udp_bridge.h
#if defined(_WIN32)
        if (errorcode == WSAEWOULDBLOCK)
#else
        if (errorcode == EWOULDBLOCK || errorcode == EAGAIN)    // Need to check both as per unix manual
#endif
        {
            ret = 0;
        }
        else
        {
            throw_system_error("recvfrom failed");
        }
    }

    return ret;
}


 void UdpBridge::reply(const uint8_t* buffer, int len, int flags)
 {
    int ret = sendto(m_sock, reinterpret_cast<const char*>(buffer), len, flags, &m_last_packet_addr, sizeof(m_last_packet_addr));

    if (ret < 0)
    {
        throw_system_error("sendto failed");
    }
 }
 