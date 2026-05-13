#include <AUnit.h>
#include <FDCxCapSense.h>

using namespace aunit;

static FDCxFeatureVector makeDirectFeatures(FDCxStatus status = FDCxStatus::Ok) {
  FDCxSample sample = FDCxSample::directFDC1004(0, 3.0f, status, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.02f, 0.01f, 0, true);
  features.compensatedDeltaPf = features.deltaPf;
  features.temperatureC = -1.0f;
  features.hasTemperature = true;
  return features;
}

static FDCxFeatureVector makeDerivedFeatures() {
  FDCxSample sample;
  sample.family = FDCxFamily::FDC2x1x_ResonantLC;
  sample.model = FDCxMeasurementModel::DerivedCapacitance;
  sample.hasCapacitance = true;
  sample.capacitanceIsDirect = false;
  sample.capacitanceIsDerived = true;
  sample.capacitancePf = 3.0f;
  sample.status = FDCxStatus::Ok;
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.02f, 0.01f, 0, true);
  features.compensatedDeltaPf = features.deltaPf;
  features.temperatureC = -1.0f;
  features.hasTemperature = true;
  return features;
}

test(ProfileRejectsUnknownFamily) {
  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  FDCxFeatureVector features = makeDirectFeatures();
  features.sample.family = FDCxFamily::Unknown;
  FDCxProfileResult result = surface.update(features);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(result.status));
}

test(ProfileRejectsUnknownModel) {
  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  FDCxFeatureVector features = makeDirectFeatures();
  features.sample.model = FDCxMeasurementModel::Unknown;
  FDCxProfileResult result = surface.update(features);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(result.status));
}

test(ProfileRejectsMissingCapacitance) {
  FDCxLiquidLevelConfig cfg;
  FDCxLiquidLevelProfile liquid(cfg);
  FDCxFeatureVector features = makeDirectFeatures();
  features.sample.hasCapacitance = false;
  FDCxProfileResult result = liquid.update(features);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(result.status));
}

test(FDC1004SampleIsDirectCapacitance) {
  FDC1004Reading reading;
  reading.channel = 2;
  reading.capacitancePf = 4.25f;
  reading.status = FDCxStatus::Ok;
  FDCxSample sample = reading.toSample();
  assertEqual(static_cast<int>(FDCxFamily::FDC1004_DirectCDC), static_cast<int>(sample.family));
  assertEqual(static_cast<int>(FDCxMeasurementModel::DirectCapacitance), static_cast<int>(sample.model));
  assertTrue(sample.hasCapacitance);
  assertTrue(sample.capacitanceIsDirect);
  assertFalse(sample.capacitanceIsDerived);
}

test(DerivedCapacitanceIsNotDirect) {
  FDCxFeatureVector features = makeDerivedFeatures();
  assertFalse(fdcxSampleHasDirectCapacitance(features.sample));
  assertFalse(fdcxProfileInputIsDirectFDC1004Capacitance(features));
}

test(ProfileRejectsDerivedCapacitanceWithoutExplicitSupport) {
  FDCxFeatureVector features = makeDerivedFeatures();
  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  FDCxProfileResult result = surface.update(features);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(result.status));
}

test(AllProfilesRejectDerivedCapacitance) {
  FDCxFeatureVector features = makeDerivedFeatures();
  FDCxLiquidLevelConfig liquidConfig;
  FDCxLiquidLevelProfile liquid(liquidConfig);
  FDCxProximityConfig proximityConfig;
  FDCxProximityProfile proximity(proximityConfig);
  FDCxGranularConfig granularConfig;
  FDCxPowderGranularProfile granular(granularConfig);
  FDCxAutomotiveProximityConfig automotiveConfig;
  FDCxAutomotiveProximityProfile automotive(automotiveConfig);
  FDCxMaterialSignature signature;
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(liquid.update(features).status));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(proximity.update(features).status));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(granular.update(features).status));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(automotive.update(features, 10).status));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(computeMaterialSignature(features, signature)));
}

test(ProfilesRejectNonOkSamples) {
  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  FDCxProximityConfig proximityConfig;
  FDCxProximityProfile proximity(proximityConfig);
  FDCxGranularConfig granularConfig;
  FDCxPowderGranularProfile granular(granularConfig);
  FDCxAutomotiveProximityConfig automotiveConfig;
  FDCxAutomotiveProximityProfile automotive(automotiveConfig);
  FDCxFeatureVector timeoutFeatures = makeDirectFeatures(FDCxStatus::Timeout);
  FDCxFeatureVector i2cFeatures = makeDirectFeatures(FDCxStatus::I2cError);
  FDCxFeatureVector notReadyFeatures = makeDirectFeatures(FDCxStatus::MeasurementNotReady);
  FDCxMaterialSignature signature;
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(surface.update(timeoutFeatures).status));
  assertEqual(static_cast<int>(FDCxStatus::I2cError), static_cast<int>(proximity.update(i2cFeatures).status));
  assertEqual(static_cast<int>(FDCxStatus::MeasurementNotReady), static_cast<int>(granular.update(notReadyFeatures).status));
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(automotive.update(timeoutFeatures, 10).status));
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(computeMaterialSignature(timeoutFeatures, signature)));
}

test(ProfileUnknownWhenBaselineNotLearned) {
  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  FDCxFeatureVector features = makeDirectFeatures();
  features.baselineLearned = false;
  FDCxProfileResult result = surface.update(features);
  assertEqual(static_cast<int>(FDCxStatus::MeasurementNotReady), static_cast<int>(result.status));
  assertEqual(static_cast<int>(FDCxProfileState::Unknown), static_cast<int>(result.state));
}

test(ProfileUnknownOnDiagnosticBaselineNotLearned) {
  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  FDCxFeatureVector features = makeDirectFeatures();
  features.sample.diagnosticFlags = fdcxFlag(FDCxDiagnosticFlag::BaselineNotLearned);
  FDCxProfileResult result = surface.update(features);
  assertEqual(static_cast<int>(FDCxProfileState::Unknown), static_cast<int>(result.state));
}

test(ConfidenceRange) {
  FDCxProximityConfig cfg;
  cfg.debounceSamples = 1;
  FDCxProximityProfile proximity(cfg);
  FDCxFeatureVector features = makeDirectFeatures();
  features.compensatedDeltaPf = 5.0f;
  FDCxProfileResult result = proximity.update(features);
  assertMoreOrEqual(result.confidence, 0.0f);
  assertLessOrEqual(result.confidence, 1.0f);
}

void setup() {
  delay(1000);
  TestRunner::run();
}

void loop() {}
