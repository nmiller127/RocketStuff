#ifndef TELEM_DEFINES_H
#define TELEM_DEFINES_H

typedef struct {
  uint32_t packetCount;
  double bmp180Temp;
  double bmp180Pressure;
} telemPacket_t;

#endif // TELEM_DEFINES_H
