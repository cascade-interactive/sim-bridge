#include "main.hpp"

#include "udp_socket.hpp"
#include "serial_port.hpp"
#include "sim_packet.hpp"

#include <iostream>
#include <stdio.h>
#include <string.h>

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
        if (bytes > 0) OnInboundUdp(buffer, bytes);//{
            //printf("Received packet!\n");
            //serial.write(buffer, bytes);
        //}

        // ESP -> Bridge -> Sim
        bytes = serial.read(buffer, sizeof(buffer));
        if (bytes > 0) OnInboundSerial(buffer, bytes);//{
            //udp.send(buffer, bytes);
        //}
    }

    WSACleanup();
    return 0;
}

void OnInboundUdp(const char* data, int size) {

    if (size < sizeof(PacketHeader)) return;

    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data);

    if (header->magic != PACKET_MAGIC) {        // 0x4C594E4E ("LYNN")
        printf("ERROR: Incompatible magic.");   
        return;
    }

    if (header->version != PACKET_VERSION) {    // 0x01;
        printf("ERROR: Incompatible protocol version.");
        return;
    }

    printf("Magic:    0x%08X\n", header->magic);
    printf("Version:  %u\n",     header->version);
    printf("Length:   %u\n",     header->length);
    printf("Flags:    0x%02X\n", header->flags);
    printf("Sequence: %u\n",     header->sequence);

    // data + sizeof(PacketHeader) is where payload starts
    const TelemetryPayload* payload = reinterpret_cast<const TelemetryPayload*>(data + sizeof(PacketHeader));

    printf("Sim Time: %.3f\n",  payload->sim_time_s);

}

void OnInboundSerial(const char* data, int size) {

}
