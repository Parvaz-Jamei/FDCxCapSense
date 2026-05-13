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

  Serial.println(fdcx_capsense::projectName());
  Serial.println(fdcx_capsense::phaseStatus());

  fdc.setTimeoutMs(150);
  FDCxStatus status = fdc.begin(Wire);
  if (status != FDCxStatus::Ok) {
    Serial.print("FDC1004 not detected: ");
    Serial.println(fdcxStatusToString(status));
    return;
  }

  // CAPDAC disabled path: CHB=b111, CAPDAC=0. This is the safest basic read setup.
  status = fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN1);
  if (status != FDCxStatus::Ok) {
    Serial.print("Configuration failed: ");
    Serial.println(fdcxStatusToString(status));
    return;
  }

  status = fdc.setSampleRate(FDC1004Rate::SPS_100);
  if (status != FDCxStatus::Ok) {
    Serial.print("Sample-rate setup failed: ");
    Serial.println(fdcxStatusToString(status));
  }
}

void loop() {
  int32_t raw = 0;
  float pf = 0.0f;

  const FDCxStatus status = fdc.readSingleMeasurement(FDC1004Measurement::M1, raw, pf);
  if (status == FDCxStatus::Ok) {
    Serial.print("raw24=");
    Serial.print(raw);
    Serial.print(" capacitance_pf=");
    Serial.println(pf, 6);
  } else {
    Serial.print("read failed: ");
    Serial.println(fdcxStatusToString(status));
  }

  delay(1000);
}
