#include <SFE_BMP180.h>
#include <Wire.h>
#include "../telemDefines.h"

#define BMP180_OVERSAMPLE_RES 2 // oversampling setting, from 0 to 3 (3 = highest res, longest wait)

// global telemetry container declared in main file
extern telemPacket_t telem;

// Context info for BMP180 readings
typedef struct {
  uint32_t pressureStartTime;
  uint32_t tempStartTime;
  uint8_t pressureTimeToWait;
  uint8_t tempTimeToWait;
  bool tempInProgress;
  bool pressureInProgress;
} bmp180Context_t;

// bmp180 context info
bmp180Context_t bmp180Context = {0};
SFE_BMP180 bmp180;

/*
 * Starts a temperature reading on the bmp180
 * Returns true for sucess, false for failure
 */
bool startBmp180Temp(){
  char timeToWait = bmp180.startTemperature();
  if(timeToWait > 0){
    bmp180Context.tempStartTime = millis();
    bmp180Context.tempTimeToWait = timeToWait;
    bmp180Context.tempInProgress = true;  // true if taking temperature, false for pressure
    return true;
  }
  return false;
}

 /*
  * Starts a pressure reading on the bmp180
  * returns true for success, false for failure
  */
bool startBmp180Pressure(){
  char timeToWait = bmp180.startPressure(BMP180_OVERSAMPLE_RES);
  if(timeToWait > 0){
    bmp180Context.pressureStartTime = millis();
    bmp180Context.pressureTimeToWait = timeToWait;
    bmp180Context.tempInProgress = false;
    return true;
  }
  return false;
}

/*
 * Checks if a temperature reading is available and stores it in telemetry if so
 */
 void checkBmp180TempStatus(){
  if(millis() - bmp180Context.tempStartTime > bmp180Context.tempTimeToWait){    // if a temperature reading is ready
    double readValue = 0.0;
    char successfulRead = bmp180.getTemperature(readValue);                     // read the temperature value
    if(successfulRead){
      telem.bmp180.temperature = (int16_t)(readValue * 10);                     // store in telemetry (10ths of a degree c) if read succeeds 
    }
    // TODO: else handle error
    if(!startBmp180Pressure()){                                                 // start a pressure reading
      // TODO: handle error
      Serial.println("BMP180 start pressure failed");
    }
  }
}

 /*
 * Checks if a pressure reading is available and stores it in telemetry if so
 */
 void checkBmp180PressureStatus(){
  if(millis() - bmp180Context.pressureStartTime > bmp180Context.pressureTimeToWait){ // if a pressure reading is ready
    double readValue = 0.0;
    double previousTemp = (double)telem.bmp180.temperature;
    char successfulRead = bmp180.getPressure(readValue, previousTemp);              // read the pressure value
    if(successfulRead){
      telem.bmp180.pressure = (uint32_t)(readValue * 100);                          // convert hPa to Pa and store in telemetry if read succeeds 
    }
    // TODO: else handle error
    if(!startBmp180Temp()){                                                         // start another pressure reading
      // TODO: handle error
      Serial.println("BMP180 start temperature failed");
    }
  }
}

