#include <FDCxCapSense.h>

FDCxSurfaceStateConfig config = {
  0.5f,  // wetDeltaThresholdPf
  1.0f,  // frostDeltaThresholdPf
  1.5f,  // iceDeltaThresholdPf
  0.10f, // maxNoisePf
  0.2f,  // minConfidence
  true   // requireTemperatureInput
};
FDCxSurfaceStateProfile profile(config);

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }
  Serial.println("Experimental metadata-aware surface-state candidate example. Not a safety system.");
}

void loop() {
  FDCxSample sample = FDCxSample::directFDC1004(0, 3.1f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.03f, 0.02f, 0, true);
  features.compensatedDeltaPf = 1.1f;
  features.temperatureC = -2.0f;
  features.hasTemperature = true;

  const FDCxProfileResult result = profile.update(features);
  Serial.print("candidate=");
  Serial.print(fdcxProfileStateToString(result.state));
  Serial.print(" reason=");
  Serial.println(result.reason);
  delay(1000);
}
