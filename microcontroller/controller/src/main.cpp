#include <Arduino.h>
#include "packet.hpp"

packet pkt;

void setup() {
  Serial.begin(115200);

  pkt.header.magic = PACKET_MAGIC;
  pkt.header.version = PACKET_VERSION;
  pkt.header.length = sizeof(PacketPayload);
  pkt.header.flags = 0x00;
  pkt.header.sequence = 0;

  pkt.payload.sim_time_s = 0.0;
  pkt.payload.accel_x = 0.0f;
  pkt.payload.accel_y = 0.0f;
  pkt.payload.accel_z = 0.0f;
  pkt.payload.gyro_x = 0.0f;
  pkt.payload.gyro_y = 0.0f;
  pkt.payload.gyro_z = 0.0f;
}

void loop() {
  pkt.header.sequence++;
  pkt.payload.sim_time_s += 0.05;
  pkt.payload.accel_z += 0.1f;

  Serial.printf("[ LOG ] Sim Time: %.1fs | Accel Z: %.2fm/s² | Seq: %u\n", 
    pkt.payload.sim_time_s, 
    pkt.payload.accel_z, 
    pkt.header.sequence);
  
    delay(100);
}