#include "FDCxProximityProfile.h"
#include "FDCxConfidence.h"
#include "../FDCxDiagnostics.h"

FDCxProximityProfile::FDCxProximityProfile(const FDCxProximityConfig& config) : _config(config) {}

FDCxProfileResult FDCxProximityProfile::update(const FDCxFeatureVector& input) {
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

  if (!fdcxProfileIsFinite(input.compensatedDeltaPf) || !fdcxProfileIsFinite(input.noisePf) ||
      !fdcxProfileIsFinite(_config.nearThresholdPf) || !fdcxProfileIsFinite(_config.farThresholdPf) ||
      !fdcxProfileIsFinite(_config.minConfidenceDeltaPf) ||
      _config.farThresholdPf > _config.nearThresholdPf || _config.nearThresholdPf <= 0.0f ||
      _config.farThresholdPf < 0.0f || _config.minConfidenceDeltaPf < 0.0f) {
    result.status = FDCxStatus::InvalidArgument;
    result.state = FDCxProfileState::Invalid;
    result.reason = "InvalidProximityConfigOrInput";
    return result;
  }
  if (fdcxHasDiagnosticFlag(input.sample.diagnosticFlags, FDCxDiagnosticFlag::SignalUnstable)) {
    result.status = FDCxStatus::MeasurementNotReady;
    result.state = FDCxProfileState::Unstable;
    result.confidence = 0.0f;
    result.reason = "SignalUnstable";
    return result;
  }

  const bool nearCandidate = input.compensatedDeltaPf >= _config.nearThresholdPf;
  const bool farCandidate = input.compensatedDeltaPf <= _config.farThresholdPf;
  if (!_near && _nearDebounce.update(nearCandidate, _config.debounceSamples)) {
    _near = true;
    _farDebounce.reset();
  } else if (_near && _farDebounce.update(farCandidate, _config.debounceSamples)) {
    _near = false;
    _nearDebounce.reset();
  }

  result.status = FDCxStatus::Ok;
  result.state = _near ? FDCxProfileState::Near : FDCxProfileState::Far;
  result.confidence = fdcxClamp01(fdcxConfidenceFromMargin(input.compensatedDeltaPf, _config.minConfidenceDeltaPf, input.noisePf));
  result.reason = _near ? "ExperimentalNearWithDebounce" : "ExperimentalFarWithHysteresis";
  return result;
}

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
FDCxProfileResult FDCxProximityProfile::update(float compensatedDeltaPf, float noisePf, uint16_t diagnosticFlags) {
  FDCxSample sample = FDCxSample::directFDC1004(0, compensatedDeltaPf, FDCxStatus::Ok, diagnosticFlags, 1.0f);
  FDCxFeatureVector input = FDCxFeatureVector::fromSample(sample, 0.0f, noisePf, 0.0f, diagnosticFlags, true);
  input.compensatedDeltaPf = compensatedDeltaPf;
  return update(input);
}
#endif
