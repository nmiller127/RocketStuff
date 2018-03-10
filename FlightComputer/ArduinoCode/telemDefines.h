#ifndef TELEM_DEFINES_H
#define TELEM_DEFINES_H

//#pragma pack(1) <- this may cause death

// temperature/pressure
typedef struct {
  int16_t temperature;  // temperature in tenths of a degree celsius
  uint32_t pressure;    // pressure in Pascals
} bmp180Telem_t;
#define BMP180_TELEM_LEN 2 + 4

// coarse accelerometer
typedef struct {
  int16_t xAccel; // 10-bit ADC readings
  int16_t yAccel;
  int16_t zAccel;
} adxl377Telem_t;
#define ADXL377_TELEM_LEN 2 + 2 + 2

// fine accel/imu
typedef struct {
  // accelerometer
  int16_t ax;
  int16_t ay;
  int16_t az;
  // gyroscope
  int16_t gx;
  int16_t gy;
  int16_t gz;
  // magnetometer
  int16_t mx;
  int16_t my;
  int16_t mz;
} mpu9250Telem_t;
#define MPU9250_TELEM_LEN 2 * 3 * 3

// main telemetry packet
typedef struct {
  uint32_t packetCount;
  bmp180Telem_t bmp180;
  adxl377Telem_t adxl377;
  mpu9250Telem_t mpu9250;
} telemPacket_t;
#define TELEM_PACKET_LEN 4 + BMP180_TELEM_LEN + ADXL377_TELEM_LEN + MPU9250_TELEM_LEN

// TODO: implement serialization/deserialization functions rather than making everything 4 bytes wide

/****************   Serialization helper functions    ****************/

// writes two bytes into the buffer pointed to by dest
void int16ToBytes(uint8_t* dest, int16_t source){
  // big endian serialization
  *dest = source >> 8;
  *(dest + 1) = source & 0xff;
}

// writes four bytes into the buffer pointed to by dest
void int32ToBytes(uint8_t* dest, int32_t source){
  // big endian serialization
  *dest = (source >> (8*3)) & 0xff;
  *(dest + 1) = (source >> (8*2)) & 0xff;
  *(dest + 2) = (source >> 8) & 0xff;
  *(dest + 3) = source & 0xff;
}

/****************   Packet Serialization    ****************/

void serializeBmp180Telem(uint8_t *& buf, bmp180Telem_t* bmp180){
  int16ToBytes(buf, bmp180->temperature);
  buf += 2;
  int32ToBytes(buf, bmp180->pressure);
  buf += 4;
}

void serializeAdxl377Telem(uint8_t *& buf, adxl377Telem_t* adxl377){
  int16ToBytes(buf, adxl377->xAccel);
  int16ToBytes(buf += 2, adxl377->yAccel);
  int16ToBytes(buf += 2, adxl377->zAccel);
  buf += 2;
}

void serializeMpu9250Telem(uint8_t *& buf, mpu9250Telem_t* mpu9250){
  int16ToBytes(buf, mpu9250->ax);
  int16ToBytes(buf += 2, mpu9250->ay);
  int16ToBytes(buf += 2, mpu9250->ax);
  int16ToBytes(buf += 2, mpu9250->gz);
  int16ToBytes(buf += 2, mpu9250->gy);
  int16ToBytes(buf += 2, mpu9250->gz);
  int16ToBytes(buf += 2, mpu9250->mx);
  int16ToBytes(buf += 2, mpu9250->my);
  int16ToBytes(buf += 2, mpu9250->mz);
  buf += 2;
}

void serializeTelemPacket(uint8_t* buf, telemPacket_t* telem){
  uint8_t* iterator = buf;
  int32ToBytes(iterator, telem->packetCount);
  iterator += 4;
  serializeBmp180Telem(iterator, &(telem->bmp180));
  serializeAdxl377Telem(iterator, &(telem->adxl377));
  serializeMpu9250Telem(iterator, &(telem->mpu9250));
}

/****************   Deserialization helper functions    ****************/

// Reads two bytes out of the buffer, forms them into an int and returns it
int16_t int16FromBytes(uint8_t* buf){
  int16_t result = 0;
  result |= *buf;
  result <<= 8;
  result |= *(buf + 1);
  return result;
}

// Reads four bytes out of the buffer, forms them into an int and returns it
int32_t int32FromBytes(uint8_t* buf){
  int32_t result = 0;
  result |= *buf;
  result <<= 8;
  result |= *(buf + 1);
  result <<= 8;
  result |= *(buf + 2);
  result <<= 8;
  result |= *(buf + 3);
  return result;
}

/****************   Packet Deserialization    ****************/

void deserializeBmp180Telem(uint8_t *& buf, bmp180Telem_t* bmp180){
  bmp180->temperature = int16FromBytes(buf);
  bmp180->pressure = (uint32_t)int32FromBytes(buf += 2);
  buf += 4;
}

void deserializeAdxl377Telem(uint8_t *& buf, adxl377Telem_t* adxl377){
  adxl377->xAccel = int16FromBytes(buf);
  adxl377->yAccel = int16FromBytes(buf += 2);
  adxl377->zAccel = int16FromBytes(buf +=2);
  buf += 2;
}

void deserializeMpu9250Telem(uint8_t *& buf, mpu9250Telem_t* mpu9250){
  mpu9250->ax = int16FromBytes(buf);
  mpu9250->ay = int16FromBytes(buf += 2);
  mpu9250->az = int16FromBytes(buf += 2);
  mpu9250->mx = int16FromBytes(buf += 2);
  mpu9250->my = int16FromBytes(buf += 2);
  mpu9250->mz = int16FromBytes(buf += 2);
  mpu9250->gx = int16FromBytes(buf += 2);
  mpu9250->gy = int16FromBytes(buf += 2);
  mpu9250->gz = int16FromBytes(buf += 2);
  buf += 2;
}

void deserializeTelemPacket(uint8_t * buf, telemPacket_t* telem){
  uint8_t* iterator = buf;
  telem->packetCount = (uint32_t)int32FromBytes(buf);
  iterator += 4;
  deserializeBmp180Telem(iterator, &(telem->bmp180));
  deserializeAdxl377Telem(iterator, &(telem->adxl377));
  deserializeMpu9250Telem(iterator, &(telem->mpu9250));
}

#endif // TELEM_DEFINES_H
