#include <Wire.h>
#include <FDCxCapSense_FDC1004.h>

FDC1004 fdc;

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }

  Wire.begin();
  fdc.setTimeoutMs(150);
  FDCxStatus status = fdc.begin(Wire);
  if (status != FDCxStatus::Ok) {
    Serial.print("FDC1004 not detected: ");
    Serial.println(fdcxStatusToString(status));
    return;
  }

  fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN1);
  fdc.setSampleRate(FDC1004Rate::SPS_100);
  fdc.idle();
}

void loop() {
  int32_t raw = 0;
  float pf = 0.0f;
  const FDCxStatus status = fdc.readSingleMeasurement(FDC1004Measurement::M1, raw, pf);
  if (status == FDCxStatus::Ok) {
    Serial.print("pf=");
    Serial.println(pf, 6);
  }

  // Conservative low-duty-cycle software pattern only. This is not a hardware current claim.
  fdc.idle();
  delay(1000);
}
