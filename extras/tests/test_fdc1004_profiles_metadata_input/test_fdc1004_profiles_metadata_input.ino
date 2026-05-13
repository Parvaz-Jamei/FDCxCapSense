#include <AUnit.h>
#include <FDCxCapSense.h>

using namespace aunit;

static FDCxFeatureVector features(float capacitancePf, float baselinePf, float noisePf, bool learned = true) {
  FDCxSample sample = FDCxSample::directFDC1004(0, capacitancePf, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector fv = FDCxFeatureVector::fromSample(sample, baselinePf, noisePf, 0.0f, 0, learned);
  fv.compensatedDeltaPf = fv.deltaPf;
  fv.temperatureC = -1.0f;
  fv.hasTemperature = true;
  return fv;
}

test(liquid_metadata_percent_clamped) {
  FDCxLiquidLevelConfig config = {2.0f, 10.0f, false, 1.0f, 0.5f};
  FDCxLiquidLevelProfile profile(config);
  FDCxFeatureVector input = features(6.0f, 2.0f, 0.01f, true);
  FDCxProfileResult result = profile.update(input);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(result.status));
  assertNear(50.0, profile.fillPercent(), 0.001);
}

test(proximity_metadata_debounce) {
  FDCxProximityConfig config = {1.0f, 0.5f, 2, 0.2f};
  FDCxProximityProfile profile(config);
  FDCxFeatureVector input = features(2.2f, 1.0f, 0.01f, true);
  input.compensatedDeltaPf = 1.2f;
  FDCxProfileResult first = profile.update(input);
  FDCxProfileResult second = profile.update(input);
  assertEqual(static_cast<int>(FDCxProfileState::Far), static_cast<int>(first.state));
  assertEqual(static_cast<int>(FDCxProfileState::Near), static_cast<int>(second.state));
}

test(surface_metadata_rejects_warm_ice) {
  FDCxSurfaceStateConfig config;
  config.minConfidence = 0.0f;
  FDCxSurfaceStateProfile profile(config);
  FDCxFeatureVector input = features(4.0f, 2.0f, 0.01f, true);
  input.compensatedDeltaPf = 2.0f;
  input.temperatureC = 10.0f;
  input.hasTemperature = true;
  FDCxProfileResult result = profile.update(input);
  assertFalse(result.state == FDCxProfileState::IceCandidate);
  assertFalse(result.state == FDCxProfileState::FrostCandidate);
}

test(granular_metadata_confidence_range) {
  FDCxGranularConfig config = {1.0f, 5.0f, 0.2f};
  FDCxPowderGranularProfile profile(config);
  FDCxFeatureVector input = features(3.0f, 0.0f, 0.01f, true);
  input.compensatedDeltaPf = 3.0f;
  FDCxProfileResult result = profile.update(input);
  assertTrue(result.confidence >= 0.0f && result.confidence <= 1.0f);
}

test(material_metadata_signature) {
  FDCxFeatureVector input = features(6.0f, 5.0f, 0.01f, true);
  input.referenceDeltaPf = 0.5f;
  FDCxMaterialSignature signature;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(computeMaterialSignature(input, signature)));
  assertTrue(signature.normalizedDelta > 0.0f);
}

test(automotive_metadata_candidate) {
  FDCxAutomotiveProximityConfig config = {1.0f, 2.0f, 2, 0.2f};
  FDCxAutomotiveProximityProfile profile(config);
  FDCxFeatureVector input = features(3.5f, 1.0f, 0.01f, true);
  input.compensatedDeltaPf = 2.5f;
  profile.update(input, 10);
  FDCxProfileResult result = profile.update(input, 100);
  assertEqual(static_cast<int>(FDCxProfileState::Candidate), static_cast<int>(result.state));
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {}
