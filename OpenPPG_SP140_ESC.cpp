/*!
 * @file OpenPPG_SP140_ESC.cpp
 *
 * @mainpage OpenPPG SP140 ESC library
 *
 * @section intro_sec Introduction
 *
 * This is a library for our SP140 line of ESCs
 *
 *
 * It parses telemetry data from UART/Serial
 *
 * @section author Author
 *
 * Written by Zach Whitehead and Braedin Butler for OpenPPG.
 *
 * @section license License
 *
 * BSD license, all text above must be included in any redistribution
 *
 */

#include "OpenPPG_SP140_ESC.h"

//#define ESC_DEBUG

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Instantiates ESC with Hardware Serial
    @param  hs Pointer to HardwareSerial object

*/
/**************************************************************************/
OpenPPG_SP140_ESC::OpenPPG_SP140_ESC(HardwareSerial *hs) {
  hwSerial = hs;
  mySerial = hwSerial;
}

/**************************************************************************/
/*!
    @brief  Instantiates ESC with a stream for Serial
    @param  serial Pointer to a Stream object

*/
/**************************************************************************/
OpenPPG_SP140_ESC::OpenPPG_SP140_ESC(Stream *serial) {
  hwSerial = NULL;

  mySerial = serial;
}

/**************************************************************************/
/*!
    @brief  Initializes serial interface and baud rate
    @param  baudrate ESCs UART baud rate (usually 57600, 9600 or 115200)
*/
/**************************************************************************/
void OpenPPG_SP140_ESC::begin(uint32_t baudrate) {
  hwSerial->begin(baudrate);
  hwSerial->setTimeout(ESC_TIMEOUT);

  volts = 0;
  prevVolts = 0;
  temperatureC = 0;
  amps = 0;
  prevAmps = 0;
  kilowatts = 0;
  prevKilowatts = 0;
  eRPM = 0;
  inPWM = 0;
  outPWM = 0;
}

bool OpenPPG_SP140_ESC::updateData() {
  uint16_t timer = 0;

  while (!mySerial->available()) {
    delay(1);
    timer++;
    if (timer >= DEFAULT_TIMEOUT) {
#ifdef ESC_DEBUG
      Serial.println("Timed out");
#endif
      return false;
    }
  }

  mySerial->readBytes(escData, ESC_DATA_SIZE);
  enforceChecksum(escData);
  parseData(escData);
  return true;
}

/**************************************************************************/
/*!
    @brief   Helper function to enforce checksum
    @param   escData Byte array
*/
/**************************************************************************/
bool OpenPPG_SP140_ESC::enforceChecksum(byte escData[]) {
  // Check checksum
  word checksum = (unsigned short)(word(escData[19], escData[18]));
  unsigned char sum1 = 0;
  unsigned char sum2 = 0;
  unsigned short sum = 0;
  for (int i = 0; i < ESC_DATA_SIZE - 2; i++) {
    sum1 = (unsigned char)(sum1 + escData[i]);
    sum2 = (unsigned char)(sum2 + sum1);
  }
  sum = (unsigned char)(sum1 - sum2);
  sum = sum << 8;
  sum |= (unsigned char)(sum2 - 2 * sum1);
#ifdef ESC_DEBUG
  Serial.print(F("     SUM: "));
  Serial.println(sum);
  Serial.print(sum1, HEX);
  Serial.print(" ");
  Serial.println(sum2, HEX);
  Serial.print(F("CHECKSUM: "));
  Serial.println(checksum);
#endif

  if (sum != checksum) {
#ifdef ESC_DEBUG
    Serial.println(F("___________________________________CHECKSUM FAILED!"));
#endif
    return false;
  }
  return true;
}

/**************************************************************************/
/*!
    @brief   Parse ESC data into variables
    @param   escData Byte array
*/
/**************************************************************************/
bool OpenPPG_SP140_ESC::parseData(byte escData[]) {
  uint16_t _volts = 0;
  uint16_t _temperatureC = 0;
  int16_t _amps = 0;
  uint32_t _eRPM = 0;
  uint16_t _inPWM = 0;
  uint16_t _outPWM = 0;

  // LSB First
  _volts = word(escData[1], escData[0]);
  //_volts = ((unsigned int)escData[1] << 8) + escData[0];
  volts = _volts / 100.0;
// reading 23.00 = 22.7 actual
// reading 16.00 = 15.17 actual
#ifdef ESC_DEBUG
  Serial.print(F("Volts: "));
  Serial.println(volts);
#endif

  /*
    batteryPercent = mapf(volts, MINIMUM_VOLTAGE, MAXIMUM_VOLTAGE, 0.0, 100.0);
    if(batteryPercent < 0){
      batteryPercent = 0;
    }
    if(batteryPercent >100){
      batteryPercent = 100;
    }
  */

  _temperatureC = word(escData[3], escData[2]);
  temperatureC = _temperatureC / 100.0;

#ifdef ESC_DEBUG
  Serial.print(F("TemperatureC: "));
  Serial.println(temperatureC);
#endif

  _amps = word(escData[5], escData[4]);
  amps = _amps / 10.0;

#ifdef ESC_DEBUG
  Serial.print(F("Amps: "));
  Serial.println(amps);
#endif

  kilowatts = amps * volts / 1000.0;

  // 7 and 6 are reserved bytes
  _eRPM = escData[11];
  _eRPM << 8;
  _eRPM += escData[10];
  _eRPM << 8;
  _eRPM += escData[9];
  _eRPM << 8;
  _eRPM += escData[8];
  eRPM = _eRPM / 6.0 / 2.0;

#ifdef ESC_DEBUG
  Serial.print(F("eRPM: "));
  Serial.println(eRPM);
#endif

  _inPWM = word(escData[13], escData[12]);
  inPWM = _inPWM / 100.0;

#ifdef ESC_DEBUG
  Serial.print(F("inPWM: "));
  Serial.println(inPWM);
#endif

  _outPWM = word(escData[15], escData[14]);
  outPWM = _outPWM / 100.0;

#ifdef ESC_DEBUG
  Serial.print(F("outPWM: "));
  Serial.println(outPWM);
#endif

  // 17 and 16 are reserved bytes
  // 19 and 18 is checksum
  word checksum = word(escData[19], escData[18]);
#ifdef ESC_DEBUG
  Serial.print(F("CHECKSUM: "));
  Serial.print(escData[19]);
  Serial.print(F(" + "));
  Serial.print(escData[18]);
  Serial.print(F(" = "));
  Serial.println(checksum);
#endif
  return true;
}
