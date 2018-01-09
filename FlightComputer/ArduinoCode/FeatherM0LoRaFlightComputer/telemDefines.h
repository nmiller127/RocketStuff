#ifndef TELEM_DEFINES_H
#define TELEM_DEFINES_H

// temperature/pressure
typedef struct {
  int32_t temperature;
  uint32_t pressure;
} bmp180Telem_t;

// coarse accelerometer
typedef struct {
  int32_t xAccel;
  int32_t yAccel;
  int32_t zAccel;
} adxl377Telem_t;

// fine accel/imu
typedef struct {
  // accelerometer
  int32_t ax;
  int32_t ay;
  int32_t az;
  // gyroscope
  int32_t gx;
  int32_t gy;
  int32_t gz;
  // magnetometer
  int32_t mx;
  int32_t my;
  int32_t mz;
} mpu9250Telem_t;

// main telemetry packet
typedef struct {
  uint32_t packetCount;
  bmp180Telem_t bmp180;
  adxl377Telem_t adxl377;
  mpu9250Telem_t mpu9250;
} telemPacket_t;

// TODO: implement serialization/deserialization functions rather than making everything 4 bytes wide

#endif // TELEM_DEFINES_H
