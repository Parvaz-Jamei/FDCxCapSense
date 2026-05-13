#include <FDCxCapSense.h>

FDCxProximityConfig config = {
  1.0f,  // nearThresholdPf
  0.5f,  // farThresholdPf
  3,     // debounceSamples
  0.2f   // minConfidenceDeltaPf
};
FDCxProximityProfile profile(config);

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }
  Serial.println("Experimental metadata-aware proximity profile.");
}

void loop() {
  FDCxSample sample = FDCxSample::directFDC1004(0, 3.2f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.03f, 0.01f, 0, true);
  features.compensatedDeltaPf = features.deltaPf;

  const FDCxProfileResult result = profile.update(features);
  Serial.print("state=");
  Serial.print(fdcxProfileStateToString(result.state));
  Serial.print(" confidence=");
  Serial.println(result.confidence, 3);
  delay(1000);
}
