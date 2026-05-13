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

  const FDC1004AutoCapdacResult result = fdc.autoTuneCapdac(
      FDC1004Measurement::M1,
      FDC1004Channel::CIN1,
      0,
      31,
      0.0f,
      0.25f);

  Serial.print("auto CAPDAC status=");
  Serial.print(fdcxStatusToString(result.status));
  Serial.print(" code=");
  Serial.print(result.capdacCode);
  Serial.print(" reason=");
  Serial.println(fdc1004CapdacReasonToString(result.reason));
}

void loop() {
  int32_t raw = 0;
  float pf = 0.0f;
  const FDCxStatus status = fdc.readSingleMeasurement(FDC1004Measurement::M1, raw, pf);
  if (status == FDCxStatus::Ok) {
    Serial.print("pf=");
    Serial.println(pf, 6);
  }
  delay(1000);
}
