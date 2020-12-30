#include <OpenPPG_SP140_ESC.h>

#define mySerial Serial1
// #define ESC_DEBUG true

OpenPPG_SP140_ESC openppgEsc = OpenPPG_SP140_ESC(&mySerial);

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for PC serial USB connection
  openppgEsc.begin(); // Initialize the ESC data
}

void loop() {
 bool updated = openppgEsc.updateData(); // try to update ESC telemetry

 Serial.print("volts: "); Serial.println(openppgEsc.volts);
 Serial.print("amps: "); Serial.println(openppgEsc.amps);
 delay(1000);

}
