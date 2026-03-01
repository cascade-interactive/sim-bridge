#include "udp_socket.hpp"

UDPSocket::UDPSocket()
    : m_socket(INVALID_SOCKET)
    , m_localAddr{}
    , m_destAddr{}
{
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

UDPSocket::~UDPSocket()
{
    close();
}

UDPSocket::UDPSocket(UDPSocket&& other) noexcept
    : m_socket(other.m_socket)
    , m_localAddr(other.m_localAddr)
    , m_destAddr(other.m_destAddr)
{
    other.m_socket = INVALID_SOCKET;
}

UDPSocket& UDPSocket::operator=(UDPSocket&& other) noexcept
{
    if (this != &other) {
        close();
        m_socket = other.m_socket;
        m_localAddr = other.m_localAddr;
        m_destAddr = other.m_destAddr;
        other.m_socket = INVALID_SOCKET;
    }
    return *this;
}

bool UDPSocket::bind(const std::string& ip, int port)
{
    if (!isValid()) return false;

    m_localAddr.sin_family = AF_INET;
    m_localAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &m_localAddr.sin_addr);

    return ::bind(m_socket, reinterpret_cast<sockaddr*>(&m_localAddr), sizeof(m_localAddr)) != SOCKET_ERROR;
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
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

bool UDPSocket::send(const void* data, int size)
{
    if (!isValid()) return false;

    int result = sendto(m_socket, static_cast<const char*>(data), size, 0,
        reinterpret_cast<const sockaddr*>(&m_destAddr), sizeof(m_destAddr));

    return result != SOCKET_ERROR;
}

int UDPSocket::receive(void* buffer, int bufferSize)
{
    if (!isValid()) return -1;

    sockaddr_in senderAddr{};
    int senderAddrSize = sizeof(senderAddr);

    return recvfrom(m_socket, static_cast<char*>(buffer), bufferSize, 0,
        reinterpret_cast<sockaddr*>(&senderAddr), &senderAddrSize);
}

bool UDPSocket::isValid() const
{
    return m_socket != INVALID_SOCKET;
}

SOCKET UDPSocket::getSocket() const
{
    return m_socket;
}