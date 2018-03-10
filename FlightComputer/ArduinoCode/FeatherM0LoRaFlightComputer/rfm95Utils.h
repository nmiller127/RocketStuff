#include "../telemDefines.h"

/* for feather m0 */ 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Transmit/Receive frequency
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// global telemetry container declared in main file
extern telemPacket_t telem;
uint8_t serializedTelemBuffer[TELEM_PACKET_LEN];

/*
 * Perform initialization of LoRa radio;
 * should be called once during init()
 */
void initializeRadio(){
  // configure pins
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manually reset the radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // initialize to default settings
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // set frequency to 915 MHz
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Set maximum transmit power
  rf95.setTxPower(23, false);
}

/*
 * Sends the current telemetry packet to the receiver
 */
bool sendTelem(){
  // serialize telemetry packet
  serializeTelemPacket(serializedTelemBuffer, &telem);

  // this call should block until the radio is ready to send, then send
  bool result = rf95.send(serializedTelemBuffer, TELEM_PACKET_LEN);
  telem.packetCount++;
  
  return result;
}
