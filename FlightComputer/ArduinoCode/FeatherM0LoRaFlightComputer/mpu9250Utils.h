#include <I2C.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"

#define MPU9250_NUM_READINGS_PER_AVG 3

MPU6050 imu; // mpu9250 object

// global telemetry container declared in main file
extern telemPacket_t telem;

// initialize imu
void mpu9250Init(){
  Wire.begin();
  imu.initialize(); // TODO: return value?
}

// read all 3 axes of accel, gyro, mag
void readMpu9250(){
  // totals used for averaging
  int32_t totalAx = 0, totalAy = 0, totalAz = 0;
  int32_t totalGx = 0, totalGy = 0, totalGz = 0;
  int32_t totalMx = 0, totalMy = 0, totalMz = 0;
  // used to hold individual readings
  int16_t ax = 0, ay = 0, az = 0;
  int16_t gx = 0, gy = 0, gz = 0;
  int16_t mx = 0, my = 0, mz = 0;
  // take and average readings
  for(uint8_t i = 0; i < MPU9250_NUM_READINGS_PER_AVG; ++i){
    imu.getMotion9( &ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz );  // TODO: return value?
    totalAx += ax; totalAy += ay; totalAz += az;
    totalGx += gx; totalGy += gy; totalGz += gz;
    totalMx += mx; totalMy += my; totalMz += mz;
  }
  // update main telemetry
  telem.mpu9250.ax = (int16_t)(totalAx / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.ay = (int16_t)(totalAy / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.az = (int16_t)(totalAz / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.gx = (int16_t)(totalGx / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.gy = (int16_t)(totalGy / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.gz = (int16_t)(totalGz / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.mx = (int16_t)(totalMx / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.my = (int16_t)(totalMy / MPU9250_NUM_READINGS_PER_AVG);
  telem.mpu9250.mz = (int16_t)(totalMz / MPU9250_NUM_READINGS_PER_AVG);
  
  /*imu.getMotion9( &telem.mpu9250.ax, &telem.mpu9250.ay, &telem.mpu9250.az,
                  &telem.mpu9250.gx, &telem.mpu9250.gy, &telem.mpu9250.gz,
                  &telem.mpu9250.mx, &telem.mpu9250.my, &telem.mpu9250.mz );  // TODO: return value?*/
}

