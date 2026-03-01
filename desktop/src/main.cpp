#include "udp_socket.hpp"
#include "serial_port.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>

// UDP
constexpr int           BRIDGE_PORT = 5000;
constexpr int           SIM_PORT    = 5001;
// Serial
constexpr const char*   COM_PORT    = "COM3";
constexpr int           BAUD_RATE   = 115200;

int main() {

    // Boilerplate WinSock implementation
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Open UDP socket
    UDPSocket udp;
    udp.bind("127.0.0.1", BRIDGE_PORT);
    udp.setDestination("127.0.0.1", SIM_PORT);

    // Open serial port
    SerialPort serial;
    serial.open(COM_PORT, BAUD_RATE);

    char buffer[256];

    while (true) {
        // Sim -> Bridge -> ESP
        int bytes = udp.receive(buffer, sizeof(buffer));
        if (bytes > 0) {
            printf("Received packet!\n");
            serial.write(buffer, bytes);
        }

        // ESP -> Bridge -> Sim
        bytes = serial.read(buffer, sizeof(buffer));
        if (bytes > 0) {
            udp.send(buffer, bytes);
        }
    }

    WSACleanup();
    return 0;
}


