#pragma once

#include "FDCxProfileTypes.h"
#include "ProfileInput.h"

struct FDCxSurfaceStateConfig {
  FDCxSurfaceStateConfig() = default;
  FDCxSurfaceStateConfig(float wet, float frost, float ice, float maxNoise, float minConf, bool requireTemp)
      : wetDeltaThresholdPf(wet),
        frostDeltaThresholdPf(frost),
        iceDeltaThresholdPf(ice),
        maxNoisePf(maxNoise),
        minConfidence(minConf),
        requireTemperatureInput(requireTemp) {}

  float wetDeltaThresholdPf = 0.5f;
  float frostDeltaThresholdPf = 1.0f;
  float iceDeltaThresholdPf = 1.5f;
  float maxNoisePf = 0.10f;
  float minConfidence = 0.5f;
  bool requireTemperatureInput = true;
  float frostMaxTemperatureC = 2.0f;
  float iceMaxTemperatureC = 0.5f;
};

class FDCxSurfaceStateProfile {
 public:
  explicit FDCxSurfaceStateProfile(const FDCxSurfaceStateConfig& config);

  // Official Roadmap v6 API.
  FDCxProfileResult update(const FDCxFeatureVector& input);

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
  // Legacy/helper API kept for compatibility only. Official examples must use update(FDCxFeatureVector).
  FDCxProfileResult update(float compensatedDeltaPf,
                           float temperatureC,
                           float noisePf,
                           float driftPf,
                           uint16_t diagnosticFlags);
#endif

 private:
  FDCxSurfaceStateConfig _config;
};
