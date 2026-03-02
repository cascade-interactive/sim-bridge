#pragma once
#include <cstdint>

static constexpr uint32_t PACKET_MAGIC   = 0x4C594E4E; // "LYNN"
static constexpr uint8_t  PACKET_VERSION = 0x01;

enum class PacketFlags : uint8_t {
    NONE        = 0x00, // Future proofing
};

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;    // 0x4C594E4E ("LYNN")
    uint8_t  version;  // Protocol version
    uint16_t length;   // Payload length in bytes
    uint8_t  flags;    // PacketFlags bitmask
    uint16_t sequence; // Packet sequence number
};

struct Packet {
    PacketHeader header;
    uint8_t*     payload; // 'header.length' bytes
    uint32_t     crc;     // CRC32 over header + payload
};

// ──────────────────── Actuator command (UDP OUT) ──────────────────────────────
struct ActuatorCommand {
    float tvc_pitch_deg;    // TVC gimbal pitch   (deg, + → nose up)
    float tvc_yaw_deg;      // TVC gimbal yaw     (deg, + → nose right)
    float canard_roll_deg;  // Canard deflection   (deg, + → clockwise from behind)
};

// ──────────────────── Telemetry payload (UDP IN) ────────────────────────────
struct TelemetryPayload {
    // Time
    double sim_time_s;

    // IMU (body frame)
    float accel_x, accel_y, accel_z;   // m/s²
    float gyro_x,  gyro_y,  gyro_z;    // rad/s

    // Barometer
    float pressure_pa;
    float baro_alt_m;

    // Magnetometer (body frame)
    float mag_x, mag_y, mag_z;         // µT

    // GPS (geodetic / NED velocities)
    double gps_lat_deg;
    double gps_lon_deg;
    float  gps_alt_m;
    float  gps_vel_n;
    float  gps_vel_e;
    float  gps_vel_d;

    // Orientation quaternion (body→world, Hamilton, scalar-first)
    float quat_w, quat_x, quat_y, quat_z;

    // Engine state
    float thrust_n;
    float chamber_pressure_pa;
    float fuel_remaining_pct;           // 0.0 – 1.0
};

#pragma pack(pop)

// ──────────────────── Packet-type identifiers ────────────────────────────────
static constexpr uint8_t PTYPE_TELEMETRY = 0x01;
static constexpr uint8_t PTYPE_ACTUATOR  = 0x02;