// Legacy/helper-only profile API coverage. Official examples and v6 tests use FDCxFeatureVector.
#include <AUnit.h>
#include <FDCxCapSense.h>

using namespace aunit;

test(liquid_percent_clamped) {
  FDCxLiquidLevelConfig config = {2.0f, 10.0f, false, 1.0f, 0.5f};
  FDCxLiquidLevelProfile profile(config);
  FDCxBaselineState baseline;
  baseline.learned = true;
  baseline.baselinePf = 2.0f;
  FDCxProfileResult result = profile.update(6.0f, 0.0f, 25.0f, baseline, 0);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(result.status));
  assertNear(50.0, profile.fillPercent(), 0.001);
}

test(liquid_invalid_span) {
  FDCxLiquidLevelConfig config = {2.0f, 2.1f, false, 1.0f, 0.5f};
  FDCxLiquidLevelProfile profile(config);
  FDCxBaselineState baseline;
  baseline.learned = true;
  FDCxProfileResult result = profile.update(2.05f, 0.0f, 25.0f, baseline, 0);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(result.status));
}

test(proximity_debounce) {
  FDCxProximityConfig config = {1.0f, 0.5f, 2, 0.2f};
  FDCxProximityProfile profile(config);
  FDCxProfileResult first = profile.update(1.2f, 0.01f, 0);
  FDCxProfileResult second = profile.update(1.2f, 0.01f, 0);
  assertEqual(static_cast<int>(FDCxProfileState::Far), static_cast<int>(first.state));
  assertEqual(static_cast<int>(FDCxProfileState::Near), static_cast<int>(second.state));
}

test(surface_unknown_on_missing_baseline) {
  FDCxSurfaceStateConfig config;
  FDCxSurfaceStateProfile profile(config);
  uint16_t flags = 0;
  fdcxSetDiagnosticFlag(flags, FDCxDiagnosticFlag::BaselineNotLearned);
  FDCxProfileResult result = profile.update(2.0f, -1.0f, 0.01f, 0.0f, flags);
  assertEqual(static_cast<int>(FDCxProfileState::Unknown), static_cast<int>(result.state));
}

test(surface_high_noise_unstable) {
  FDCxSurfaceStateConfig config;
  FDCxSurfaceStateProfile profile(config);
  FDCxProfileResult result = profile.update(2.0f, -1.0f, 1.0f, 0.0f, 0);
  assertEqual(static_cast<int>(FDCxProfileState::Unstable), static_cast<int>(result.state));
}

test(material_signature) {
  FDCxMaterialSignature signature;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(computeMaterialSignature(6.0f, 5.0f, 2.5f, 2.0f, signature)));
  assertTrue(signature.normalizedDelta > 0.0f);
}

test(confidence_range) {
  FDCxGranularConfig config = {1.0f, 5.0f, 0.2f};
  FDCxPowderGranularProfile profile(config);
  FDCxProfileResult result = profile.update(3.0f, 0.01f, 0);
  assertTrue(result.confidence >= 0.0f && result.confidence <= 1.0f);
}

test(automotive_demo_candidate) {
  FDCxAutomotiveProximityConfig config = {1.0f, 2.0f, 2, 0.2f};
  FDCxAutomotiveProximityProfile profile(config);
  profile.update(2.5f, 0.01f, 10);
  FDCxProfileResult result = profile.update(2.5f, 0.01f, 20);
  assertEqual(static_cast<int>(FDCxProfileState::Candidate), static_cast<int>(result.state));
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {}
