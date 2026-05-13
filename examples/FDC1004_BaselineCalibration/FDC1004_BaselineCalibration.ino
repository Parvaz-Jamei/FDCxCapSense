#include <Wire.h>
#include <FDCxCapSense_FDC1004.h>

FDC1004 fdc;
FDCxBaselineConfig baselineConfig = {
  8,      // warmupSamples
  8,      // minStableSamples
  0.05f,  // maxNoisePf
  0.02f,  // learningRate
  1.0f    // driftLimitPf
};
FDCxBaselineTracker baseline(baselineConfig);

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
}

void loop() {
  int32_t raw = 0;
  float pf = 0.0f;
  const FDCxStatus status = fdc.readSingleMeasurement(FDC1004Measurement::M1, raw, pf);
  if (status == FDCxStatus::Ok) {
    const bool accepted = baseline.update(pf);
    const FDCxBaselineState state = baseline.state();
    Serial.print("pf=");
    Serial.print(pf, 6);
    Serial.print(" baseline=");
    Serial.print(state.baselinePf, 6);
    Serial.print(" learned=");
    Serial.print(state.learned ? "yes" : "no");
    Serial.print(" accepted=");
    Serial.println(accepted ? "yes" : "no");
  }
  delay(1000);
}
