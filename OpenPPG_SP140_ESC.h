#ifndef OpenPPG_SP140_ESC_H
#define OpenPPG_SP140_ESC_H

/*!
 * @file OpenPPG_SP140_ESC.h
 */

#include "Arduino.h"

#define ESC_BAUD_RATE 115200
#define ESC_DATA_SIZE 20
#define ESC_TIMEOUT 10

//#define ESC_DEBUG

#define DEFAULT_TIMEOUT 200 //!< UART reading timeout in milliseconds

///! Helper class to communicate with and keep state with ESC
class OpenPPG_SP140_ESC {
public:
  OpenPPG_SP140_ESC(HardwareSerial *hs);
  OpenPPG_SP140_ESC(Stream *serial);

  void begin(uint32_t baud = ESC_BAUD_RATE);

  bool updateData(void); // main function to be called

  bool enforceChecksum(byte escData[]);
  bool parseData(byte escData[]);

  // ESC Telemetry
  float volts;
  float temperatureC;
  float amps;
  float kilowatts;
  float eRPM;
  float inPWM;
  float outPWM;

private:
  byte escData[ESC_DATA_SIZE];
  Stream *mySerial;
  HardwareSerial *hwSerial;
};

#endif