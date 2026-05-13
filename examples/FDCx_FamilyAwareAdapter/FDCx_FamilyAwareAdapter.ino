#include <FDCxCapSense.h>

void setup() {
  Serial.begin(115200);
  FDC2x1xFrequency frequency;
  frequency.status = FDCxStatus::Ok;
  frequency.frequencyHz = 1000000.0f;
  frequency.channel = FDC2x1xChannel::CH0;

  FDCxSample frequencySample = frequency.toSample();
  Serial.println(frequencySample.model == FDCxMeasurementModel::ResonantFrequency ? "frequency sample" : "unexpected");

  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(frequencySample, 0.0f, 0.01f, 0.0f, 0, true);
  FDCxProfileResult result = surface.update(features);
  Serial.println(result.status == FDCxStatus::InvalidArgument ? "FDC1004-only profile rejected FDC2x1x sample" : "unexpected");
}

void loop() {}
