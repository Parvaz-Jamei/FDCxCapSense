#include <AUnit.h>
#include <FDCxCapSense.h>
#include "../fake_i2c/FakeI2C.h"

using namespace aunit;

test(auto_capdac_invalid_bounds) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  FDC1004AutoCapdacResult result = fdc.autoTuneCapdac(FDC1004Measurement::M1, FDC1004Channel::CIN1, 10, 2, 0.0f, 0.1f);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(result.status));
}

test(auto_capdac_normal_result) {
  FakeI2C bus;
  bus.setSimulatedCapacitance(0, 5.0f);
  FDC1004 fdc;
  fdc.setTimeoutMs(20);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  FDC1004AutoCapdacResult result = fdc.autoTuneCapdac(FDC1004Measurement::M1, FDC1004Channel::CIN1, 0, 3, 5.0f, 0.01f);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(result.status));
  assertFalse(result.saturated);
}

test(auto_capdac_saturated_high) {
  FakeI2C bus;
  bus.setSimulatedCapacitance(0, 200.0f);
  FDC1004 fdc;
  fdc.setTimeoutMs(20);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  FDC1004AutoCapdacResult result = fdc.autoTuneCapdac(FDC1004Measurement::M1, FDC1004Channel::CIN1, 0, 1, 0.0f, 0.01f);
  assertTrue(result.saturated);
  assertEqual(static_cast<int>(FDC1004CapdacReason::SaturatedHigh), static_cast<int>(result.reason));
}

test(baseline_warmup_and_stable_learning) {
  FDCxBaselineConfig config;
  config.warmupSamples = 3;
  config.minStableSamples = 2;
  config.maxNoisePf = 0.05f;
  FDCxBaselineTracker tracker(config);
  assertFalse(tracker.update(10.0f));
  assertFalse(tracker.update(10.01f));
  assertFalse(tracker.state().learned);
  tracker.update(10.0f);
  tracker.update(10.01f);
  assertTrue(tracker.state().learned);
}

test(baseline_rejects_unstable_data) {
  FDCxBaselineConfig config;
  config.warmupSamples = 2;
  config.minStableSamples = 2;
  config.maxNoisePf = 0.02f;
  FDCxBaselineTracker tracker(config);
  tracker.update(1.0f);
  tracker.update(5.0f);
  tracker.update(1.0f);
  assertFalse(tracker.state().learned);
}

test(reference_compensation_formula) {
  FDCxReferenceCompensation out = compensateWithReference(12.0f, 10.0f, 4.0f, 3.0f, 0.5f);
  assertNear(2.0, out.primaryDeltaPf, 0.0001);
  assertNear(1.0, out.referenceDeltaPf, 0.0001);
  assertNear(1.5, out.compensatedDeltaPf, 0.0001);
}

test(temperature_compensation_formula) {
  FDCxTemperatureCompensationConfig config;
  config.referenceTemperatureC = 25.0f;
  config.alphaPfPerC = 0.1f;
  float out = 0.0f;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(compensateTemperatureLinearChecked(10.0f, 30.0f, config, out)));
  assertNear(9.5, out, 0.0001);
}

test(diagnostics_flags) {
  FDCxDiagnosticReport report = fdcxBuildDiagnosticReport(10.0f, 9.0f, 0.2f, false, 31, 8000000L, 0.85f, 0.10f, true);
  assertTrue(fdcxHasDiagnosticFlag(report.flags, FDCxDiagnosticFlag::BaselineNotLearned));
  assertTrue(fdcxHasDiagnosticFlag(report.flags, FDCxDiagnosticFlag::SignalUnstable));
  assertTrue(fdcxHasDiagnosticFlag(report.flags, FDCxDiagnosticFlag::CapdacAtMaximum));
  assertTrue(fdcxHasDiagnosticFlag(report.flags, FDCxDiagnosticFlag::RawNearPositiveLimit));
}

test(profile_round_trip) {
  FDC1004CalibrationProfile profile;
  profile.capdacCode[0] = 3;
  profile.baselinePf[0] = 12.5f;
  profile.alphaPfPerC[0] = 0.02f;
  uint8_t buffer[kFDC1004CalibrationProfileSerializedSize] = {};
  FDC1004CalibrationProfile decoded;
  assertTrue(fdcxSerializeProfile(profile, buffer, sizeof(buffer)));
  assertTrue(fdcxDeserializeProfile(buffer, sizeof(buffer), decoded));
  assertEqual(3, static_cast<int>(decoded.capdacCode[0]));
  assertNear(12.5, decoded.baselinePf[0], 0.0001);
  assertNear(0.02, decoded.alphaPfPerC[0], 0.0001);
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {}
