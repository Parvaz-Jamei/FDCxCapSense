#include <FDCxCapSense_FDC1004.h>
#include <Wire.h>

FDC1004 fdc;

void setup() {
  Serial.begin(115200);
#if defined(ESP32)
  Wire.begin(21, 22);
#else
  Wire.begin();
#endif
  fdc.begin(Wire);
  Serial.println("Optional ESP32 custom Wire pattern; core library has no FreeRTOS dependency.");
}

void loop() {
  delay(1000);
}
