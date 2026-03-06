#pragma once
#include <cstdint>

static constexpr uint32_t PACKET_MAGIC   = 0x4C594E4E; // "LYNN"
static constexpr uint8_t  PACKET_VERSION = 0x01;

#pragma pack(push, 1)

struct PacketHeader {
    uint32_t magic;    // 0x4C594E4E ("LYNN")
    uint8_t  version;  // Protocol version
    uint16_t length;   // Payload length in bytes
    uint8_t  flags;    // PacketFlags bitmask
    uint16_t sequence; // Packet sequence number
};

struct PacketPayload {
    double sim_time_s;

    // IMU
    float accel_x, accel_y, accel_z;   // m/s²
    float gyro_x,  gyro_y,  gyro_z;    // rad/s
};

struct packet {
    PacketHeader        header;
    PacketPayload       payload; 
    uint32_t            crc;          
};

#pragma pack(pop)