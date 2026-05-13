#pragma once

#include "FDCxProfileTypes.h"
#include "ProfileInput.h"

struct FDCxGranularConfig {
  FDCxGranularConfig() = default;
  FDCxGranularConfig(float empty, float full, float maxNoise)
      : emptyPf(empty), fullPf(full), maxNoisePf(maxNoise) {}

  float emptyPf = 0.0f;
  float fullPf = 1.0f;
  float maxNoisePf = 0.10f;
};

class FDCxPowderGranularProfile {
 public:
  explicit FDCxPowderGranularProfile(const FDCxGranularConfig& config);

  // Official Roadmap v6 API.
  FDCxProfileResult update(const FDCxFeatureVector& input);

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
  // Legacy/helper API kept for compatibility only. Official examples must use update(FDCxFeatureVector).
  FDCxProfileResult update(float compensatedPf, float noisePf, uint16_t diagnosticFlags);
#endif

 private:
  FDCxGranularConfig _config;
};
