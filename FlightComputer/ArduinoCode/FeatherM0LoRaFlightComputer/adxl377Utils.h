#include "../telemDefines.h"

#define ADXL377_X_PIN A1 // Analog 1
#define ADXL377_Y_PIN A2 // Analog 2
#define ADXL377_Z_PIN A3 // Analog 3
#define ADXL377_NUM_READINGS_PER_AVG 3  // number of individual sensor reading to avaerage together per telem packet

// global telemetry container declared in main file
extern telemPacket_t telem;

void adxl377Init(){
  analogReference(AR_EXTERNAL);  // Set external reference voltage
                              // (tied to 3v pin on sensor)
  // Configure input pins
  pinMode(ADXL377_X_PIN, INPUT);
  pinMode(ADXL377_Y_PIN, INPUT);
  pinMode(ADXL377_Z_PIN, INPUT);
}

double getAdxl377Reading(uint8_t pinNumber){
  uint16_t analogReading = 0;
  for(uint8_t i = 0; i < ADXL377_NUM_READINGS_PER_AVG; ++i){
    analogReading += analogRead(pinNumber);
  }
  //Serial.print("ADXL377 reading: "); Serial.println(analogReading);
  return (int32_t)(analogReading / ADXL377_NUM_READINGS_PER_AVG);
}

void readAdxl377(){
  telem.adxl377.xAccel = getAdxl377Reading(ADXL377_X_PIN);
  telem.adxl377.yAccel = getAdxl377Reading(ADXL377_Y_PIN);
  telem.adxl377.zAccel = getAdxl377Reading(ADXL377_Z_PIN);
}

