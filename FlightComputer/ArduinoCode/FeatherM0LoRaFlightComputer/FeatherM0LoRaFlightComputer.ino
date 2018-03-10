#include <SPI.h>
#include <RH_RF95.h>
#include "rfm95Utils.h"
#include "../telemDefines.h"

// inclues for sensors
#include "bmp180Utils.h"
#include "adxl377Utils.h"
#include "mpu9250Utils.h"
#include "gpsUtils.h"
#include <Adafruit_GPS.h>

// Period for timed portion of main loop
#define MAIN_LOOP_PERIOD_MS 1000 // 1 Hz telemetry updates

telemPacket_t telem = {0};  // main telemetry container

void setup() 
{
  // TODO: Just for debug for now
  //while (!Serial);
  Serial.begin(9600);
  delay(3000);

  // initialize LoRa radio
  initializeRadio();

  // initialize bmp180 object
  if(!bmp180.begin()){
    Serial.println("BMP180 initialization failed");
    //while(1); // TODO: replace with something better
  }

  // initialize coarse accelerometer
  adxl377Init();

  // initialize imu
  mpu9250Init();

  // initialize the gps
  gpsInit();

  // Start temperature and pressure readings
  if(!startBmp180Temp()){
    // TODO: handle error
    Serial.println("BMP180 start temperature failed");
  }
}

// variables used for timed portion of main loop
uint32_t lastLoopTime = 0;
uint32_t currentTime;

void loop()
{
  // Read one at a time; temperature then pressure
  if(bmp180Context.tempInProgress){
    checkBmp180TempStatus(); // Check if there's a temperature reading available and record it if so
  } else {
    checkBmp180PressureStatus(); // Check if there's a pressure reading available and record it if so
  }

  // hand-query the gps
  readGps();
  
  currentTime = millis();
  if (currentTime - lastLoopTime >= MAIN_LOOP_PERIOD_MS){
    // TODO: remove before flight
    int32_t diff = currentTime - lastLoopTime;
    if(diff > MAIN_LOOP_PERIOD_MS){
      Serial.print("Warning: missed main loop timing taget by "); Serial.print(diff - MAIN_LOOP_PERIOD_MS); Serial.println(" ms.");
    }
    // end RBF
    lastLoopTime = currentTime;
    
    /********   1 Hz portion of main loop   ********/

    // send telemetry
    if(sendTelem()){
      Serial.println("Sent a packet");
    } else {
      Serial.println("Send telem failed");
    }

    // update adxl377 values
    readAdxl377();

    // update mpu9250 values
    readMpu9250();
  }
}
 
