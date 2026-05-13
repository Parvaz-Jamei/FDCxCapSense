#include "FDCxPowderGranularProfile.h"
#include "FDCxConfidence.h"
#include "../FDCxDiagnostics.h"

FDCxPowderGranularProfile::FDCxPowderGranularProfile(const FDCxGranularConfig& config) : _config(config) {}

FDCxProfileResult FDCxPowderGranularProfile::update(const FDCxFeatureVector& input) {
  FDCxProfileResult result;
  result.metrics = fdcxMetricsFromFeatures(input);

  FDCxProfileResult inputGuard = fdcxRejectIfNotDirectFDC1004(input);
  if (inputGuard.status != FDCxStatus::Ok) {
    inputGuard.metrics = result.metrics;
    return inputGuard;
  }
  FDCxProfileResult noiseGuard = fdcxRejectIfInvalidNoise(input);
  if (noiseGuard.status != FDCxStatus::Ok) {
    noiseGuard.metrics = result.metrics;
    return noiseGuard;
  }
  const float span = _config.fullPf - _config.emptyPf;
  if (!fdcxProfileIsFinite(input.compensatedDeltaPf) || !fdcxProfileIsFinite(input.noisePf) ||
      !fdcxProfileIsFinite(_config.emptyPf) || !fdcxProfileIsFinite(_config.fullPf) ||
      !fdcxProfileIsFinite(_config.maxNoisePf) || _config.maxNoisePf < 0.0f || span <= 0.0f) {
    result.status = FDCxStatus::InvalidArgument;
    result.state = FDCxProfileState::Invalid;
    result.reason = "InvalidGranularInputOrConfig";
    return result;
  }
  if (input.noisePf > _config.maxNoisePf || fdcxHasDiagnosticFlag(input.sample.diagnosticFlags, FDCxDiagnosticFlag::SignalUnstable)) {
    result.status = FDCxStatus::MeasurementNotReady;
    result.state = FDCxProfileState::Unstable;
    result.reason = "GranularProxyUnstable";
    return result;
  }
  const float pct = (input.compensatedDeltaPf - _config.emptyPf) / span;
  result.status = FDCxStatus::Ok;
  result.confidence = fdcxClamp01(pct);
  result.state = pct <= 0.05f ? FDCxProfileState::Empty : (pct >= 0.95f ? FDCxProfileState::Full : FDCxProfileState::Partial);
  result.reason = "ExperimentalGranularProxy";
  return result;
}

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
FDCxProfileResult FDCxPowderGranularProfile::update(float compensatedPf, float noisePf, uint16_t diagnosticFlags) {
  FDCxSample sample = FDCxSample::directFDC1004(0, compensatedPf, FDCxStatus::Ok, diagnosticFlags, 1.0f);
  FDCxFeatureVector input = FDCxFeatureVector::fromSample(sample, 0.0f, noisePf, 0.0f, diagnosticFlags, true);
  input.compensatedDeltaPf = compensatedPf;
  return update(input);
}
#endif
