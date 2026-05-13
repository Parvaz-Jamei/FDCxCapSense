#pragma once

#include "FDCxProfileTypes.h"
#include "FDCxDebounce.h"
#include "ProfileInput.h"

struct FDCxAutomotiveProximityConfig {
  FDCxAutomotiveProximityConfig() = default;
  FDCxAutomotiveProximityConfig(float handThreshold, float eventThreshold, uint8_t debounce, float maxNoise)
      : handPresenceThresholdPf(handThreshold),
        touchlessCandidateThresholdPf(eventThreshold),
        debounceSamples(debounce),
        maxNoisePf(maxNoise) {}

  float handPresenceThresholdPf = 1.0f;
  float touchlessCandidateThresholdPf = 2.0f;
  uint8_t debounceSamples = 3;
  float maxNoisePf = 0.20f;
  uint32_t minCandidateIntervalMs = 80;
};

class FDCxAutomotiveProximityProfile {
 public:
  explicit FDCxAutomotiveProximityProfile(const FDCxAutomotiveProximityConfig& config);

  // Official Roadmap v6 API. timestampMs uses unsigned-delta comparisons so millis() wrap-around does not create false rejects.
  FDCxProfileResult update(const FDCxFeatureVector& input, uint32_t timestampMs);

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
  // Legacy/helper API kept for compatibility only. Official examples must use update(FDCxFeatureVector, timestampMs).
  FDCxProfileResult update(float compensatedDeltaPf, float noisePf, uint32_t timestampMs);
#endif

 private:
  FDCxAutomotiveProximityConfig _config;
  FDCxDebounce _presenceDebounce;
  FDCxDebounce _eventDebounce;
  uint32_t _lastTimestampMs = 0;
  uint32_t _lastCandidateTimestampMs = 0;
  bool _hasTimestamp = false;
};
