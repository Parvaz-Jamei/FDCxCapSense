#include "FDCxLiquidLevelProfile.h"
#include "FDCxConfidence.h"
#include "../FDC1004Calibration.h"
#include "../FDCxDiagnostics.h"

namespace {
float clampPercent(float value) {
  if (value < 0.0f) return 0.0f;
  if (value > 100.0f) return 100.0f;
  return value;
}
}

FDCxLiquidLevelProfile::FDCxLiquidLevelProfile(const FDCxLiquidLevelConfig& config) : _config(config) {}

FDCxProfileResult FDCxLiquidLevelProfile::update(const FDCxFeatureVector& input) {
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
  if (!fdcxProfileIsFinite(input.sample.capacitancePf) || !fdcxProfileIsFinite(_config.emptyPf) ||
      !fdcxProfileIsFinite(_config.fullPf) || !fdcxProfileIsFinite(_config.referenceScale) ||
      !fdcxProfileIsFinite(_config.minValidSpanPf) || _config.minValidSpanPf <= 0.0f ||
      !fdcxProfileIsFinite(span) || fdcxProfileAbs(span) < _config.minValidSpanPf) {
    result.status = FDCxStatus::InvalidArgument;
    result.state = FDCxProfileState::Invalid;
    result.reason = "InvalidLiquidSpanOrInput";
    return result;
  }
  if (!fdcxProfileBaselineReady(input)) {
    result.status = FDCxStatus::MeasurementNotReady;
    result.state = FDCxProfileState::Unknown;
    result.reason = "BaselineNotLearned";
    return result;
  }

  float compensatedPf = input.sample.capacitancePf;
  if (_config.useReferenceChannel) {
    if (!_config.referenceBaselineValid) {
      result.status = FDCxStatus::MeasurementNotReady;
      result.state = FDCxProfileState::Unknown;
      result.reason = "ReferenceBaselineNotReady";
      return result;
    }
    if (!input.hasReference || !fdcxProfileIsFinite(input.referenceDeltaPf)) {
      result.status = FDCxStatus::MeasurementNotReady;
      result.state = FDCxProfileState::Unknown;
      result.reason = "ReferenceDeltaRequired";
      return result;
    }
    // Roadmap v6: no hardcoded reference delta. Metadata-aware reference mode requires a valid reference delta.
    const float referenceDelta = input.referenceDeltaPf;
    result.metrics.referenceDeltaPf = referenceDelta;
    result.metrics.compensatedDeltaPf = input.deltaPf - (referenceDelta * _config.referenceScale);
    compensatedPf = input.baselinePf + result.metrics.compensatedDeltaPf;
  } else {
    result.metrics.compensatedDeltaPf = input.compensatedDeltaPf;
    compensatedPf = input.baselinePf + input.compensatedDeltaPf;
  }

  _fillPercent = clampPercent(((compensatedPf - _config.emptyPf) / span) * 100.0f);
  result.status = FDCxStatus::Ok;
  result.state = (_fillPercent <= 1.0f) ? FDCxProfileState::Empty : (_fillPercent >= 99.0f ? FDCxProfileState::Full : FDCxProfileState::Partial);
  result.confidence = fdcxClamp01(fdcxProfileAbs(span) / (fdcxProfileAbs(span) + input.noisePf + 0.001f));
  result.reason = "ExperimentalLiquidLevelPercent";
  return result;
}

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
FDCxProfileResult FDCxLiquidLevelProfile::update(float levelPf,
                                                 float referencePf,
                                                 float temperatureC,
                                                 const FDCxBaselineState& baseline,
                                                 uint16_t diagnosticFlags) {
  FDCxSample sample = FDCxSample::directFDC1004(0, levelPf, FDCxStatus::Ok, diagnosticFlags, 1.0f);
  FDCxFeatureVector input = FDCxFeatureVector::fromSample(sample, baseline.baselinePf, baseline.noisePf, baseline.driftPf, diagnosticFlags, baseline.learned);
  // Compatibility note: temperatureC is metadata only in this legacy helper.
  // Apply temperature compensation before creating FDCxFeatureVector for official flows.
  input.temperatureC = temperatureC;
  input.hasTemperature = fdcxProfileIsFinite(temperatureC);
  if (_config.useReferenceChannel) {
    if (!_config.referenceBaselineValid) {
      FDCxProfileResult result;
      result.metrics = fdcxMetricsFromFeatures(input);
      result.status = FDCxStatus::MeasurementNotReady;
      result.state = FDCxProfileState::Unknown;
      result.reason = "ReferenceBaselineNotReady";
      return result;
    }
    input.hasReference = true;
    input.referenceDeltaPf = referencePf - _config.referenceBaselinePf;
  }
  return update(input);
}
#endif

float FDCxLiquidLevelProfile::fillPercent() const {
  return _fillPercent;
}
