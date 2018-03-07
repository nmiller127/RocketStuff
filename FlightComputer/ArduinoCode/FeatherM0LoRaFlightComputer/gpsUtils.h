#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
#include <Adafruit_GPS.h>

#define GPS_RX_PIN 11 // from feather's perspective; connected to gps's TX
#define GPS_TX_PIN 10  // connected to gps's RX

Uart gpsSerial(&sercom1, GPS_RX_PIN, GPS_TX_PIN, SERCOM_RX_PAD_0, UART_TX_PAD_2);
Adafruit_GPS gps(&gpsSerial);

/*
 * Initialize the GPS
 */
void gpsInit(){
  gps.begin(9600); // 9600 is default baud rate for gps
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // request rmc and gga sentences
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // set 1 Hz update rate
}

/*
 * Called continuously in loop
 * Reads a character at a time and parses nmea sentences as they arrive
 */
void readGps(){
  char c = gps.read(); // read in a character
  // parse NMEA sentence when a new one arrives
  if(gps.newNMEAreceived()){
    if(gps.parse(gps.lastNMEA())){
      ; // todo: update telemetry
    } else {
      ; // todo handle error
    }
  }
}

