#include <Wire.h>
#include <FDCxCapSense_FDC1004.h>

FDC1004 fdc;
FDCxTemperatureCompensationConfig tempConfig;

float readExternalTemperatureC() {
  // Replace this stub with an external temperature sensor read in the final system.
  return 25.0f;
}

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }

  Wire.begin();
  tempConfig.referenceTemperatureC = 25.0f;
  tempConfig.alphaPfPerC = 0.0f;  // User-calibrated coefficient; never hardcoded by the library.

  fdc.setTimeoutMs(150);
  FDCxStatus status = fdc.begin(Wire);
  if (status != FDCxStatus::Ok) {
    Serial.print("FDC1004 not detected: ");
    Serial.println(fdcxStatusToString(status));
    return;
  }

  fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN1);
  fdc.setSampleRate(FDC1004Rate::SPS_100);
}

void loop() {
  int32_t raw = 0;
  float pf = 0.0f;
  const FDCxStatus status = fdc.readSingleMeasurement(FDC1004Measurement::M1, raw, pf);
  if (status == FDCxStatus::Ok) {
    float compensated = 0.0f;
    const float temperatureC = readExternalTemperatureC();
    const FDCxStatus compStatus = compensateTemperatureLinearChecked(pf, temperatureC, tempConfig, compensated);
    if (compStatus == FDCxStatus::Ok) {
      Serial.print("pf=");
      Serial.print(pf, 6);
      Serial.print(" tempC=");
      Serial.print(temperatureC, 2);
      Serial.print(" compensatedPf=");
      Serial.println(compensated, 6);
    }
  }
  delay(1000);
}
