#include <FDCxCapSense.h>

FDCxLiquidLevelConfig config = {
  2.0f,   // emptyPf, user-calibrated
  10.0f,  // fullPf, user-calibrated
  true,   // useReferenceChannel
  1.0f,   // referenceScale
  0.5f    // minValidSpanPf
};
FDCxLiquidLevelProfile profile(config);

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }
  Serial.println("Experimental metadata-aware liquid-level profile. Not field validated.");
}

void loop() {
  FDCxSample sample = FDCxSample::directFDC1004(0, 6.0f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.02f, 0.01f, 0, true);
  features.hasReference = true;
  features.referenceDeltaPf = 0.10f;
  features.temperatureC = 25.0f;
  features.hasTemperature = true;

  const FDCxProfileResult result = profile.update(features);
  Serial.print("state=");
  Serial.print(fdcxProfileStateToString(result.state));
  Serial.print(" fillPercent=");
  Serial.println(profile.fillPercent(), 2);
  delay(1000);
}
