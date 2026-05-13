#pragma once

#include "FDCxProfileTypes.h"
#include "ProfileInput.h"
#include "../FDCxBaseline.h"

struct FDCxLiquidLevelConfig {
  FDCxLiquidLevelConfig() = default;
  FDCxLiquidLevelConfig(float empty, float full, bool useReference, float refScale, float minSpan)
      : emptyPf(empty), fullPf(full), useReferenceChannel(useReference), referenceScale(refScale), minValidSpanPf(minSpan) {}

  float emptyPf = 0.0f;
  float fullPf = 1.0f;
  bool useReferenceChannel = false;
  float referenceScale = 1.0f;
  float minValidSpanPf = 0.5f;
  float referenceBaselinePf = 0.0f;
  bool referenceBaselineValid = false;
};

class FDCxLiquidLevelProfile {
 public:
  explicit FDCxLiquidLevelProfile(const FDCxLiquidLevelConfig& config);

  // Official Roadmap v6 API: metadata-aware input only.
  FDCxProfileResult update(const FDCxFeatureVector& input);

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
  // Legacy/helper API kept for compatibility only. Official examples must use update(FDCxFeatureVector).
  // temperatureC is metadata only in this helper; apply temperature compensation before
  // creating FDCxFeatureVector when compensated liquid-level behavior is required.
  FDCxProfileResult update(float levelPf,
                           float referencePf,
                           float temperatureC,
                           const FDCxBaselineState& baseline,
                           uint16_t diagnosticFlags);
#endif

  float fillPercent() const;

 private:
  FDCxLiquidLevelConfig _config;
  float _fillPercent = 0.0f;
};
