#include "FDCxSurfaceStateProfile.h"
#include "FDCxConfidence.h"
#include "../FDCxDiagnostics.h"

FDCxSurfaceStateProfile::FDCxSurfaceStateProfile(const FDCxSurfaceStateConfig& config) : _config(config) {}

FDCxProfileResult FDCxSurfaceStateProfile::update(const FDCxFeatureVector& input) {
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

  if (!fdcxProfileIsFinite(input.compensatedDeltaPf) || !fdcxProfileIsFinite(input.noisePf) || !fdcxProfileIsFinite(input.driftPf) ||
      !fdcxProfileIsFinite(_config.wetDeltaThresholdPf) || !fdcxProfileIsFinite(_config.frostDeltaThresholdPf) ||
      !fdcxProfileIsFinite(_config.iceDeltaThresholdPf) || !fdcxProfileIsFinite(_config.maxNoisePf) ||
      !fdcxProfileIsFinite(_config.minConfidence) || !fdcxProfileIsFinite(_config.frostMaxTemperatureC) ||
      !fdcxProfileIsFinite(_config.iceMaxTemperatureC) || _config.maxNoisePf < 0.0f ||
      _config.minConfidence < 0.0f || _config.minConfidence > 1.0f ||
      _config.wetDeltaThresholdPf < 0.0f || _config.frostDeltaThresholdPf < _config.wetDeltaThresholdPf ||
      _config.iceDeltaThresholdPf < _config.frostDeltaThresholdPf || _config.iceMaxTemperatureC > _config.frostMaxTemperatureC) {
    result.status = FDCxStatus::InvalidArgument;
    result.state = FDCxProfileState::Invalid;
    result.reason = "InvalidSurfaceInputOrConfig";
    return result;
  }
  if (_config.requireTemperatureInput && (!input.hasTemperature || !fdcxProfileIsFinite(input.temperatureC))) {
    result.status = FDCxStatus::MeasurementNotReady;
    result.state = FDCxProfileState::Unknown;
    result.reason = "TemperatureRequired";
    return result;
  }
  if (!fdcxProfileBaselineReady(input)) {
    result.status = FDCxStatus::MeasurementNotReady;
    result.state = FDCxProfileState::Unknown;
    result.reason = "BaselineNotLearned";
    return result;
  }
  if (input.noisePf > _config.maxNoisePf || fdcxHasDiagnosticFlag(input.sample.diagnosticFlags, FDCxDiagnosticFlag::SignalUnstable)) {
    result.status = FDCxStatus::MeasurementNotReady;
    result.state = FDCxProfileState::Unstable;
    result.reason = "SignalUnstable";
    return result;
  }

  result.status = FDCxStatus::Ok;
  const bool allowIce = !_config.requireTemperatureInput || (input.hasTemperature && input.temperatureC <= _config.iceMaxTemperatureC);
  const bool allowFrost = !_config.requireTemperatureInput || (input.hasTemperature && input.temperatureC <= _config.frostMaxTemperatureC);
  if (input.compensatedDeltaPf >= _config.iceDeltaThresholdPf && allowIce) {
    result.state = FDCxProfileState::IceCandidate;
    result.reason = "ExperimentalIceCandidateOnly";
  } else if (input.compensatedDeltaPf >= _config.frostDeltaThresholdPf && allowFrost) {
    result.state = FDCxProfileState::FrostCandidate;
    result.reason = "ExperimentalFrostCandidateOnly";
  } else if (input.compensatedDeltaPf >= _config.wetDeltaThresholdPf) {
    result.state = FDCxProfileState::WetCandidate;
    result.reason = "ExperimentalWetCandidateOnly";
  } else {
    result.state = FDCxProfileState::Dry;
    result.reason = "BelowCandidateThresholds";
  }
  result.confidence = fdcxClamp01(fdcxConfidenceFromMargin(input.compensatedDeltaPf, _config.iceDeltaThresholdPf, input.noisePf));
  if (result.confidence < _config.minConfidence && result.state != FDCxProfileState::Dry) {
    result.state = FDCxProfileState::Unknown;
    result.reason = "BelowMinimumConfidence";
  }
  return result;
}

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
FDCxProfileResult FDCxSurfaceStateProfile::update(float compensatedDeltaPf,
                                                  float temperatureC,
                                                  float noisePf,
                                                  float driftPf,
                                                  uint16_t diagnosticFlags) {
  FDCxSample sample = FDCxSample::directFDC1004(0, compensatedDeltaPf, FDCxStatus::Ok, diagnosticFlags, 1.0f);
  FDCxFeatureVector input = FDCxFeatureVector::fromSample(sample, 0.0f, noisePf, driftPf, diagnosticFlags, true);
  input.compensatedDeltaPf = compensatedDeltaPf;
  input.deltaPf = compensatedDeltaPf;
  input.temperatureC = temperatureC;
  input.hasTemperature = fdcxProfileIsFinite(temperatureC);
  return update(input);
}
#endif
