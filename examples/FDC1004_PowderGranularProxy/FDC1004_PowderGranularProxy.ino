#include <FDCxCapSense.h>

FDCxGranularConfig config = {
  1.0f,  // emptyPf
  8.0f,  // fullPf
  0.15f  // maxNoisePf
};
FDCxPowderGranularProfile profile(config);

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }
  Serial.println("Experimental powder/granular proxy. No density or moisture claim.");
}

void loop() {
  FDCxSample sample = FDCxSample::directFDC1004(0, 4.5f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 1.0f, 0.05f, 0.01f, 0, true);
  features.compensatedDeltaPf = sample.capacitancePf;

  const FDCxProfileResult result = profile.update(features);
  Serial.print("state=");
  Serial.print(fdcxProfileStateToString(result.state));
  Serial.print(" confidence=");
  Serial.println(result.confidence, 3);
  delay(1000);
}
