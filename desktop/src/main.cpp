#include "main.hpp"

#include "udp_socket.hpp"
#include "serial_port.hpp"
#include "sim_packet.hpp"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstring> 

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

    // Min size guard
    if (size < (int)sizeof(PacketHeader)) {

        printf("ERROR: Packet too small (%d bytes).\n", size);
        return;
    }

    PacketHeader header{};
    std::memcpy(&header, data, sizeof(PacketHeader));

    if (header.magic != PACKET_MAGIC) {
        printf("ERROR: Bad magic 0x%08X (expected 0x%08X). \n", header.magic, PACKET_MAGIC);
        return;
    }

    if (header.version != PACKET_VERSION) {
        printf("ERROR: Incompatible version %u (expected %u). \n", header.magic, PACKET_VERSION);
        return;
    }

    if ((int)header.length != size) {
        printf("ERROR: Length mismatch (declared %u, received %d).\n", header.length, size);
        return;
    }

    printf("Magic:    0x%08X\n", header.magic);
    printf("Version:  %u\n",     header.version);
    printf("Length:   %u\n",     header.length);
    printf("Flags:    0x%02X\n", header.flags);
    printf("Sequence: %u\n",     header.sequence);

    // Payload
    const size_t payloadOffset = sizeof(PacketHeader);
    const size_t payloadSize   = size - payloadOffset;

    if (payloadSize < sizeof(TelemetryPayload)) {
        printf("ERROR: Payload too small (%zu bytes, need %zu).\n",
               payloadSize, sizeof(TelemetryPayload));
        return;
    }

    TelemetryPayload payload{};
    std::memcpy(&payload, data + payloadOffset, sizeof(TelemetryPayload));

    printf("Sim Time: %.3f\n", payload.sim_time_s);

    //const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data);
//
    //if (header->magic   !=  PACKET_MAGIC) {        // 0x4C594E4E ("LYNN")
    //    printf("ERROR: Incompatible magic.");   
    //    return;
    //}
//
    //if (header->version !=  PACKET_VERSION) {    // 0x01;
    //    printf("ERROR: Incompatible protocol version.");
    //    return;
    //}
//
    //if (header->flags   ==  0x00) {
    //    // Parse crc
    //    printf("ERROR: Unable to parse CRC.");
    //    return;
    //}
//
    //printf("Magic:    0x%08X\n", header->magic);
    //printf("Version:  %u\n",     header->version);
    //printf("Length:   %u\n",     header->length);
    //printf("Flags:    0x%02X\n", header->flags);
    //printf("Sequence: %u\n",     header->sequence);
//
    //// data + sizeof(PacketHeader) is where payload starts
    //const TelemetryPayload* payload = reinterpret_cast<const TelemetryPayload*>(data + sizeof(PacketHeader));
//
    //printf("Sim Time: %.3f\n",  payload->sim_time_s);
//
}

void OnInboundSerial(const char* data, int size) {

}
