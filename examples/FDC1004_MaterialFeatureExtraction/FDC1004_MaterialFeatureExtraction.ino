#include <FDCxCapSense.h>

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }
  Serial.println("Experimental material feature extraction. No material classification claim.");
}

void loop() {
  FDCxSample sample = FDCxSample::directFDC1004(0, 5.4f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 5.0f, 0.02f, 0.01f, 0, true);
  features.hasReference = true;
  features.referenceDeltaPf = 0.05f;

  FDCxMaterialSignature signature;
  if (computeMaterialSignature(features, signature) == FDCxStatus::Ok) {
    Serial.print("normalizedDelta=");
    Serial.println(signature.normalizedDelta, 6);
  }
  delay(1000);
}
