#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
#include <Adafruit_GPS.h>

#define GPS_RX_PIN 11 // from feather's perspective; connected to gps's TX
#define GPS_TX_PIN 10 // connected to gps's RX

// Command to set only GGA sentences; generated using http://www.hhhh.org/wiml/proj/nmeaxor.html
#define GPS_CMD_SET_GGA_ONLY "$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"

// global telemetry container declared in main file
extern telemPacket_t telem;

// Setup a second hardware serial port for communicating with GPS
Uart gpsSerial(&sercom1, GPS_RX_PIN, GPS_TX_PIN, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  gpsSerial.IrqHandler();
}

// GPS parsing object
Adafruit_GPS gps(&gpsSerial);

/*
 * Initialize the GPS
 */
void gpsInit(){
  gps.begin(9600); // 9600 is default baud rate for gps
  // Assign GPS pins SERCOM functionality
  pinPeripheral(GPS_TX_PIN, PIO_SERCOM);
  pinPeripheral(GPS_RX_PIN, PIO_SERCOM);

  delay(500); // Wait for serial init
  gps.sendCommand(GPS_CMD_SET_GGA_ONLY); // request only gga sentences PMTK_SET_NMEA_OUTPUT_RMCGGA
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // set 1 Hz update rate
}

/*
 * Sets the NMEA sentence buffer in telemetry to all null characters
 */
void wipeNmeaBuffer(){
  for(size_t i = 0; i < NMEA_BUFFER_LEN; ++i){
    telem.gps.nmea[i] = '\0';
  }
}

/*
 * Returns the length of the nmea sentence currently in the 
 * provided nmea buffer (nmeaBuf).
 * Returns -1 if buffer is empty/invalid.
 */
ssize_t currentNmeaLen(char* nmeaBuf){
  ssize_t len = 0;
  
  // Search until newline is found
  size_t i = 0;
  for (; i < NMEA_BUFFER_LEN; ++i){
    if (nmeaBuf[i-2] == '*'){ // * precedes a two-character checksum
      break; // found end of sentence
    }
  }

  // Return length if newline found, -1 otherwise
  if (i < NMEA_BUFFER_LEN){
    len = i + 1; // Length is last index + 1
  } else {
    len = -1; // error
  }

  return len;
}

/*
 * Copies the latest nmea sentence from the parsing library's buffer
 * to the telem buffer, wiping the telem buffer first.
 */
void updateNmeaBuffer(char* parsingBuf){
  wipeNmeaBuffer(); // Wipe the buffer currently in telem

  // Determine the length of the latest sentence
  ssize_t sentenceLength = currentNmeaLen(parsingBuf); 

  Serial.print("Sentence length: "); Serial.println(sentenceLength);

  // Copy the latest sentence into telem
  if (sentenceLength > -1){
    memcpy(telem.gps.nmea, parsingBuf, sentenceLength);
  } else {
    ; // TODO: handle error
  }
}

/*
 * Called continuously in loop
 * Reads a character at a time and parses nmea sentences as they arrive
 */
void readGps(){
  char c = gps.read(); // read in a character
  // parse NMEA sentence when a new one arrives
  if(gps.newNMEAreceived()){

    char* parsingBuf = gps.lastNMEA();
    if(gps.parse(parsingBuf)){

      parsingBuf += 1; // Discard newline at start of sentence
      updateNmeaBuffer(parsingBuf); // update telemetry
      
      // TODO: remove
      /*Serial.print("NMEA: ");
      for(size_t i = 0; i < NMEA_BUFFER_LEN; ++i){
        Serial.print(telem.gps.nmea[i]);
        if (telem.gps.nmea[i-2] == '*'){
          Serial.println();
          break;
        }
      }*/
    } else {
      ; // todo handle error
    }
  }
}
