#include "FDCxAutomotiveProximityProfile.h"
#include "FDCxConfidence.h"

namespace {
constexpr uint32_t kMaxReasonableAutomotiveTimestampGapMs = 24UL * 60UL * 60UL * 1000UL;
}


FDCxAutomotiveProximityProfile::FDCxAutomotiveProximityProfile(const FDCxAutomotiveProximityConfig& config) : _config(config) {}

FDCxProfileResult FDCxAutomotiveProximityProfile::update(const FDCxFeatureVector& input, uint32_t timestampMs) {
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
      !fdcxProfileIsFinite(_config.handPresenceThresholdPf) || !fdcxProfileIsFinite(_config.touchlessCandidateThresholdPf) ||
      !fdcxProfileIsFinite(_config.maxNoisePf) || _config.maxNoisePf < 0.0f ||
      _config.handPresenceThresholdPf <= 0.0f || _config.touchlessCandidateThresholdPf < _config.handPresenceThresholdPf) {
    result.status = FDCxStatus::InvalidArgument;
    result.state = FDCxProfileState::Invalid;
    result.reason = "InvalidAutomotiveDemoInputOrConfig";
    return result;
  }
  if (_hasTimestamp) {
    const uint32_t timestampDeltaMs = timestampMs - _lastTimestampMs;
    if (timestampDeltaMs > kMaxReasonableAutomotiveTimestampGapMs) {
      result.status = FDCxStatus::InvalidArgument;
      result.state = FDCxProfileState::Invalid;
      result.reason = "TimestampGapTooLargeOrBackward";
      return result;
    }
  }
  _hasTimestamp = true;
  _lastTimestampMs = timestampMs;

  if (input.noisePf > _config.maxNoisePf) {
    result.status = FDCxStatus::MeasurementNotReady;
    result.state = FDCxProfileState::Unstable;
    result.reason = "AutomotiveDemoNoiseTooHigh";
    return result;
  }
  const bool rawEventCandidate = input.compensatedDeltaPf >= _config.touchlessCandidateThresholdPf;
  const bool intervalOk = (_lastCandidateTimestampMs == 0u) || (timestampMs - _lastCandidateTimestampMs >= _config.minCandidateIntervalMs);
  const bool eventCandidate = _eventDebounce.update(rawEventCandidate && intervalOk, _config.debounceSamples);
  const bool presenceCandidate = _presenceDebounce.update(input.compensatedDeltaPf >= _config.handPresenceThresholdPf, _config.debounceSamples);
  result.status = FDCxStatus::Ok;
  if (eventCandidate) {
    _lastCandidateTimestampMs = timestampMs;
    result.state = FDCxProfileState::Candidate;
    result.reason = "TouchlessProximityCandidateDemo";
  } else if (presenceCandidate) {
    result.state = FDCxProfileState::Near;
    result.reason = "PresenceCandidateDemo";
  } else {
    result.state = FDCxProfileState::Far;
    result.reason = "ExperimentalAutomotiveDemoFar";
  }
  result.confidence = fdcxClamp01(fdcxConfidenceFromMargin(input.compensatedDeltaPf, _config.handPresenceThresholdPf, input.noisePf));
  return result;
}

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
FDCxProfileResult FDCxAutomotiveProximityProfile::update(float compensatedDeltaPf, float noisePf, uint32_t timestampMs) {
  FDCxSample sample = FDCxSample::directFDC1004(0, compensatedDeltaPf, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector input = FDCxFeatureVector::fromSample(sample, 0.0f, noisePf, 0.0f, 0, true);
  input.compensatedDeltaPf = compensatedDeltaPf;
  return update(input, timestampMs);
}
#endif
