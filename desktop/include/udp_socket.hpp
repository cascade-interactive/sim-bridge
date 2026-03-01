#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
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
        SOCKET getSocket() const;

    private:
        SOCKET m_socket;
        sockaddr_in m_localAddr;
        sockaddr_in m_destAddr;    
};