#include "rfm95Utils.h"
#include "../telemDefines.h"

// Container for received telemetry
telemPacket_t receivedPacket;
uint8_t telemReceiveBuffer[TELEM_PACKET_LEN];

void setup() 
{
  //while (!Serial);
  Serial.begin(9600);
  delay(100);

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
    }
    else
    {
      Serial.println("Receive failed");
    }
    Serial.println("Listening for telemetry...");
  }
}
