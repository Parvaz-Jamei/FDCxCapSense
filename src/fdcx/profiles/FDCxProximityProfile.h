#pragma once

#include "FDCxProfileTypes.h"
#include "FDCxDebounce.h"
#include "ProfileInput.h"

struct FDCxProximityConfig {
  FDCxProximityConfig() = default;
  FDCxProximityConfig(float nearThreshold, float farThreshold, uint8_t debounce, float minConfidenceDelta)
      : nearThresholdPf(nearThreshold),
        farThresholdPf(farThreshold),
        debounceSamples(debounce),
        minConfidenceDeltaPf(minConfidenceDelta) {}

  float nearThresholdPf = 1.0f;
  float farThresholdPf = 0.5f;
  uint8_t debounceSamples = 3;
  float minConfidenceDeltaPf = 0.2f;
};

class FDCxProximityProfile {
 public:
  explicit FDCxProximityProfile(const FDCxProximityConfig& config);

  // Official Roadmap v6 API.
  FDCxProfileResult update(const FDCxFeatureVector& input);

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
  // Legacy/helper API kept for compatibility only. Official examples must use update(FDCxFeatureVector).
  FDCxProfileResult update(float compensatedDeltaPf, float noisePf, uint16_t diagnosticFlags);
#endif

 private:
  FDCxProximityConfig _config;
  FDCxDebounce _nearDebounce;
  FDCxDebounce _farDebounce;
  bool _near = false;
};
