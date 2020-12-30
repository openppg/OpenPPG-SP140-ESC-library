#include <OpenPPG_SP140_ESC.h>

#define mySerial Serial1
#define ESC_DEBUG true

OpenPPG_SP140_ESC openppgEsc = OpenPPG_SP140_ESC(&mySerial);

#define ESC_DATA_SIZE 20

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("\n Testing begin");
  openppgEsc.begin();
}

void loop() {
  // Sample ESC data
  byte escData[ESC_DATA_SIZE] = {
    0xD8,
    0x8,
    0x30,
    0x8,
    0x5,
    0x0,
    0x0,
    0x0,
    0xF0,
    0x83,
    0x0,
    0x0,
    0x78,
    0x14,
    0xCE,
    0x13,
    0x0,
    0x0,
    0x1C,
    0xE7
  };

  // bool updated = openppgEsc.updateData();
  Serial.print("volts "); Serial.println(openppgEsc.volts);

  bool checksum = openppgEsc.enforceChecksum(escData);
  bool parse = openppgEsc.parseData(escData);

  Serial.print("check passed "); Serial.println(checksum);
  Serial.print("parsed "); Serial.println(parse);

  delay(1000);
}
