#include "main.hpp"

#include "udp_socket.hpp"
#include "serial_port.hpp"
#include "sim_packet.hpp"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cstring> 

#ifdef _WIN32
    #include <windows.h>
#endif

int main(int argc, char* argv[]) {

    // Open UDP socket
    UDPSocket udp;
    udp.bind("127.0.0.1", BRIDGE_PORT);
    udp.setDestination("127.0.0.1", SIM_PORT);

    // Open serial port
    SerialPort serial;
    serial.open(COM_PORT, BAUD_RATE);

    intro();

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

    return 0;
}

void OnInboundUdp(const char* data, int size) {

    // Min size guard
    if (size < sizeof(PacketHeader)) return;

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

    const int expectedSize = (int)(sizeof(PacketHeader) + header.length + sizeof(uint32_t)); // header + payload + crc32
    if (expectedSize != size) {
        printf("ERROR: Length mismatch (expected %d, received %d).\n", expectedSize, size);
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

    if (payloadSize + sizeof(VisualPayload) < sizeof(TelemetryPayload)) {   // TEMP: Figure out what to do once i have a concrete protocol
        printf("ERROR: Payload too small (%zu bytes, need %zu).\n",
               payloadSize, sizeof(TelemetryPayload));
        return;
    }

    TelemetryPayload payload{};
    std::memcpy(&payload, data + payloadOffset, sizeof(TelemetryPayload));

    printf("Sim Time: %.3f\n", payload.sim_time_s);

}

void OnInboundSerial(const char* data, int size) {

}

void intro() {

#ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8
#endif

    printf(
    "██████╗ ██████╗ ██╗██████╗  ██████╗ ███████╗    \n"
    "██╔══██╗██╔══██╗██║██╔══██╗██╔════╝ ██╔════╝    \n"
    "██████╔╝██████╔╝██║██║  ██║██║  ███╗█████╗      \n"
    "██╔══██╗██╔══██╗██║██║  ██║██║   ██║██╔══╝      \n"
    "██████╔╝██║  ██║██║██████╔╝╚██████╔╝███████╗    \n"
    "╚═════╝ ╚═╝  ╚═╝╚═╝╚═════╝  ╚═════╝ ╚══════╝    \n");

    printf("Version 0.0.0 Alpha 1\n\nAwaiting packets...\n");
}
