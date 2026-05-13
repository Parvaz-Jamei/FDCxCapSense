#include <FDCxCapSense.h>

FDCxAutomotiveProximityConfig config = {
  1.0f,  // presence threshold
  2.0f,  // touchless-proximity candidate threshold
  3,     // debounce samples
  0.20f  // max noise
};
FDCxAutomotiveProximityProfile profile(config);

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }
  Serial.println("Experimental automotive proximity demo only. No actuator or safety claim.");
}

void loop() {
  FDCxSample sample = FDCxSample::directFDC1004(0, 4.2f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.04f, 0.01f, 0, true);
  features.compensatedDeltaPf = 2.2f;

  const FDCxProfileResult result = profile.update(features, millis());
  Serial.print("demo state=");
  Serial.print(fdcxProfileStateToString(result.state));
  Serial.print(" reason=");
  Serial.println(result.reason);
  delay(250);
}
