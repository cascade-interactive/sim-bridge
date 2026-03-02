#pragma once

#include "udp_socket.hpp"
#include "serial_port.hpp"

// UDP
constexpr int           BRIDGE_PORT = 5000;
constexpr int           SIM_PORT    = 5001;
// Serial
constexpr const char*   COM_PORT    = "COM3";
constexpr int           BAUD_RATE   = 115200;

// Handlers
void OnInboundUdp(const char* data, int size);
void OnInboundSerial(const char* data, int size);
