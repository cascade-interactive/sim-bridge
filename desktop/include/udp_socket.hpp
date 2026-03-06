#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    using SocketHandle = SOCKET;
    constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;
    constexpr int kSocketError = SOCKET_ERROR;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using SocketHandle = int;
    constexpr SocketHandle kInvalidSocket = -1;
    constexpr int kSocketError = -1;
#endif

#include <string>

class UDPSocket {
    public:
        UDPSocket();
        ~UDPSocket();

        // Delete any copies
        UDPSocket(const UDPSocket&) = delete;
        UDPSocket& operator=(const UDPSocket&) = delete;

        // Move
        UDPSocket(UDPSocket&& other) noexcept;
        UDPSocket& operator=(UDPSocket&& other) noexcept;

        // Setup
        bool bind(const std::string& ip, int port);
        void setDestination(const std::string& ip, int port);
        void close();

        // I/O
        bool send(const void* data, int size);          // Returns wether send was successful
        int receive(void* buffer, int bufferSize);      // Returns number of bytes received

        // State
        bool isValid() const;
        SocketHandle getSocket() const;

    private:
        static void platformInit();
        static void platformCleanup();
        static int s_instanceCount;

        SocketHandle m_socket;
        sockaddr_in m_localAddr;
        sockaddr_in m_destAddr;    
};