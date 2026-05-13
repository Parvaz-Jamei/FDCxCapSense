#include <FDCxCapSense.h>

FDCxSurfaceStateConfig surfaceConfig;
FDCxSurfaceStateProfile surface(surfaceConfig);

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) {
    delay(10);
  }
  Serial.println("Family-aware profile input example: FDC1004Reading -> FDCxSample -> FDCxFeatureVector -> profile.update(features)");
}

void loop() {
  FDC1004Reading reading;
  reading.channel = 0;
  reading.raw24 = 0;
  reading.capacitancePf = 3.25f;
  reading.status = FDCxStatus::Ok;
  reading.diagnosticFlags = 0;

  FDCxSample sample = reading.toSample();
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.03f, 0.01f, 0, true);
  features.compensatedDeltaPf = features.deltaPf;
  features.temperatureC = -1.0f;
  features.hasTemperature = true;

  FDCxProfileResult result = surface.update(features);
  Serial.print("family=");
  Serial.print(fdcxFamilyToString(sample.family));
  Serial.print(" model=");
  Serial.print(fdcxMeasurementModelToString(sample.model));
  Serial.print(" state=");
  Serial.println(fdcxProfileStateToString(result.state));
  delay(1000);
}
