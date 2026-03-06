#include "udp_socket.hpp"

#include <cstdio>

int UDPSocket::s_instanceCount = 0;

void UDPSocket::platformInit()
{
#ifdef _WIN32
    if (s_instanceCount == 0) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            printf("ERROR: WSAStartup failed.\n");
        }
    }
#endif
    ++s_instanceCount;
}

void UDPSocket::platformCleanup()
{
    --s_instanceCount;
#ifdef _WIN32
    if (s_instanceCount == 0) {
        WSACleanup();
    }
#endif
}

UDPSocket::UDPSocket()
    : m_socket(kInvalidSocket)
    , m_localAddr{}
    , m_destAddr{}
{
    platformInit();
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

UDPSocket::~UDPSocket()
{
    close();
    platformCleanup();
}

UDPSocket::UDPSocket(UDPSocket&& other) noexcept
    : m_socket(other.m_socket)
    , m_localAddr(other.m_localAddr)
    , m_destAddr(other.m_destAddr)
{
    platformInit();
    other.m_socket = kInvalidSocket;
}

UDPSocket& UDPSocket::operator=(UDPSocket&& other) noexcept
{
    if (this != &other) {
        close();
        m_socket = other.m_socket;
        m_localAddr = other.m_localAddr;
        m_destAddr = other.m_destAddr;
        other.m_socket = kInvalidSocket;
    }
    return *this;
}

bool UDPSocket::bind(const std::string& ip, int port)
{
    if (!isValid()) return false;

    m_localAddr.sin_family = AF_INET;
    m_localAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &m_localAddr.sin_addr);

    return ::bind(m_socket, reinterpret_cast<sockaddr*>(&m_localAddr), sizeof(m_localAddr)) != kSocketError;
}

void UDPSocket::setDestination(const std::string& ip, int port)
{
    m_destAddr.sin_family = AF_INET;
    m_destAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &m_destAddr.sin_addr);
}

void UDPSocket::close()
{
    if (isValid()) {
#ifdef _WIN32
        closesocket(m_socket);
#else
        ::close(m_socket);
#endif
        m_socket = kInvalidSocket;
    }
}

bool UDPSocket::send(const void* data, int size)
{
    if (!isValid()) return false;

    int result = sendto(m_socket, static_cast<const char*>(data), size, 0,
        reinterpret_cast<const sockaddr*>(&m_destAddr), sizeof(m_destAddr));

    return result != kSocketError;
}

int UDPSocket::receive(void* buffer, int bufferSize)
{
    if (!isValid()) return -1;

    sockaddr_in senderAddr{};
#ifdef _WIN32
    int senderAddrSize = sizeof(senderAddr);
#else
    socklen_t senderAddrSize = sizeof(senderAddr);
#endif

    return recvfrom(m_socket, static_cast<char*>(buffer), bufferSize, 0,
        reinterpret_cast<sockaddr*>(&senderAddr), &senderAddrSize);
}

bool UDPSocket::isValid() const
{
    return m_socket != kInvalidSocket;
}

SocketHandle UDPSocket::getSocket() const
{
    return m_socket;
}