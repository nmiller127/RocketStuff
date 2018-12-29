#include "rfm95Utils.h"
#include "../telemDefines.h"
#include "SoftwareSerial.h"


// Container for received telemetry
telemPacket_t receivedPacket;
uint8_t telemReceiveBuffer[TELEM_PACKET_LEN];

// Template for NMEA GGA sentence
//char nmeaRxBuf[NMEA_BUFFER_LEN] = {0}; TODO: remove

SoftwareSerial gpsSerial(5,6);   // (TODO: assign pins) For spoofed GPS output
uint32_t lastGpsTxTime;
uint32_t currentTime;

void setup() 
{
  Serial.begin(9600);
  delay(100);
  gpsSerial.begin(9600);
  delay(100);
  lastGpsTxTime = 0;

  // Initialize the rfm95
  initializeRadio();

  Serial.println("Listening for telemetry...");
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    receivedPacket = {0};
    uint8_t len = TELEM_PACKET_LEN;
    
    if (rf95.recv(telemReceiveBuffer, &len))
    {
      // Deserialize received telemetry
      deserializeTelemPacket(telemReceiveBuffer, &receivedPacket);
      
      Serial.print("Packet number "); Serial.print(receivedPacket.packetCount); Serial.println(':');
      Serial.print("Temperature: "); Serial.println(receivedPacket.bmp180.temperature);
      Serial.print("Pressure: "); Serial.println(receivedPacket.bmp180.pressure);
      Serial.print("Coarse X accel: "); Serial.println(receivedPacket.adxl377.xAccel);
      Serial.print("Coarse Y accel: "); Serial.println(receivedPacket.adxl377.yAccel);
      Serial.print("Coarse Z accel: "); Serial.println(receivedPacket.adxl377.zAccel);
      Serial.print("Fine accel x: "); Serial.println(receivedPacket.mpu9250.ax);
      Serial.print("Gyro x: "); Serial.println(receivedPacket.mpu9250.gx);
      Serial.print("Mag x: "); Serial.println(receivedPacket.mpu9250.mx);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      Serial.println("");

      // Print the latest nmea sentence over the gpsSerial output
      printNmea(&receivedPacket);
    }
    else
    {
      Serial.println("Receive failed");
    }
    Serial.println("Listening for telemetry...");
  }
/* TODO: remove
  currentTime = millis();
  if (currentTime >= lastGpsTxTime + 1000)
  {
    gpsSerial.println(gpsSentence);
    lastGpsTxTime = currentTime;
  }*/
}

/*
 * Prints the nmea sentence out of the telem packet
 * pointed to by telem over the gpsSerial output
 */
void printNmea(telemPacket_t* telem){
  for (size_t i = 0; i < NMEA_BUFFER_LEN; ++i){
    // local pointer to received nmea sentence
    char* nmea = telem->gps.nmea;
    
    // check for null terminator
    /*if (nmea[i] == '\0'){
      break;
    }*/

    // Print next char
    gpsSerial.print(nmea[i]);

    // Check if that was the end of the sentence
    if (i >= 2 && nmea[i-2] == '*') {
      gpsSerial.println(""); // newline
      break;
    }
  }
}