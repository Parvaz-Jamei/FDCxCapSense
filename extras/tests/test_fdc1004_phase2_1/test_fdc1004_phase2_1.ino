#include <AUnit.h>
#include <FDCxCapSense.h>
#include "../fake_i2c/FakeI2C.h"

using namespace aunit;

test(power_idle_clears_repeat_and_triggers) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setRepeatMode(true)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.triggerSingleMeasurement(FDC1004Measurement::M1)));
  assertTrue((fdc.currentConfigRegisterValue() & fdc1004_registers::kRepeatBit) != 0u);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.idle()));
  assertEqual(0, static_cast<int>(fdc.currentConfigRegisterValue() & fdc1004_registers::kRepeatBit));
  assertEqual(0, static_cast<int>(fdc.currentConfigRegisterValue() & fdc1004_registers::kMeasurementTriggerMask));
}

test(temperature_lookup_interpolates) {
  FDCxTemperaturePoint table[] = {
    {20.0f, -0.2f},
    {30.0f, 0.2f}
  };
  float out = 0.0f;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(compensateTemperatureLookup(10.0f, 25.0f, table, 2, out)));
  assertNear(10.0, out, 0.0001);
}

test(temperature_lookup_rejects_unsorted) {
  FDCxTemperaturePoint table[] = {
    {30.0f, 0.2f},
    {20.0f, -0.2f}
  };
  float out = 0.0f;
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(compensateTemperatureLookup(10.0f, 25.0f, table, 2, out)));
}

test(auto_capdac_reports_residual) {
  FakeI2C bus;
  bus.setSimulatedCapacitance(0, 12.0f);
  FDC1004 fdc;
  fdc.setTimeoutMs(20);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  FDC1004AutoCapdacResult result = fdc.autoTuneCapdac(FDC1004Measurement::M1, FDC1004Channel::CIN1, 0, 10, 0.0f, 1.6f);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(result.status));
  assertTrue(result.residualPf <= 1.6f && result.residualPf >= -1.6f);
}


test(config_cache_not_updated_on_i2c_write_failure) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  const uint16_t before = fdc.currentConfigRegisterValue();
  bus.failWrite = true;
  assertEqual(static_cast<int>(FDCxStatus::I2cError), static_cast<int>(fdc.setRepeatMode(true)));
  assertEqual(before, fdc.currentConfigRegisterValue());
  assertEqual(static_cast<int>(FDCxStatus::I2cError), static_cast<int>(fdc.setSampleRate(FDC1004Rate::SPS_400)));
  assertEqual(before, fdc.currentConfigRegisterValue());
  assertEqual(static_cast<int>(FDCxStatus::I2cError), static_cast<int>(fdc.triggerSingleMeasurement(FDC1004Measurement::M1)));
  assertEqual(before, fdc.currentConfigRegisterValue());
}

test(read_capacitance_requires_configured_measurement) {
  FakeI2C bus;
  FDC1004 fdc;
  float pf = 1.0f;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::MeasurementNotReady), static_cast<int>(fdc.readCapacitancePf(FDC1004Measurement::M1, pf)));
  assertEqual(static_cast<float>(0.0f), pf);
}

test(diagnostics_capdac_limits_only_when_enabled) {
  FDCxDiagnosticReport disabled = fdcxBuildDiagnosticReport(10.0f, 9.0f, 0.02f, true, 0, 0, 0.85f, 0.10f, false);
  assertFalse(fdcxHasDiagnosticFlag(disabled.flags, FDCxDiagnosticFlag::CapdacAtMinimum));
  FDCxDiagnosticReport enabled = fdcxBuildDiagnosticReport(10.0f, 9.0f, 0.02f, true, 0, 0, 0.85f, 0.10f, true);
  assertTrue(fdcxHasDiagnosticFlag(enabled.flags, FDCxDiagnosticFlag::CapdacAtMinimum));
}

test(feature_vector_near_zero_baseline_flagged) {
  FDCxSample sample = FDCxSample::directFDC1004(0, 1.0f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 0.0f, 0.01f, 0.0f, 0, true);
  assertEqual(static_cast<float>(0.0f), features.normalizedDelta);
  assertTrue(fdcxHasDiagnosticFlag(features.sample.diagnosticFlags, FDCxDiagnosticFlag::BaselineNearZero));
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {}
