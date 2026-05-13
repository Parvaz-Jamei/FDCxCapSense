#pragma once

#include "FDCxSample.h"
#include "FDCxMath.h"
#include "../fdcx/FDCxDiagnostics.h"

struct FDCxFeatureVector {
  FDCxSample sample;
  float baselinePf = 0.0f;
  float deltaPf = 0.0f;
  float normalizedDelta = 0.0f;
  float compensatedDeltaPf = 0.0f;
  float referenceDeltaPf = 0.0f;
  float temperatureC = 0.0f;
  float noisePf = 0.0f;
  float driftPf = 0.0f;
  bool baselineLearned = false;
  bool hasReference = false;
  bool hasTemperature = false;

  static FDCxFeatureVector fromSample(const FDCxSample& source,
                                      float baseline,
                                      float noise,
                                      float drift,
                                      uint16_t diagnostics,
                                      bool learned = true) {
    FDCxFeatureVector features;
    features.sample = source;
    features.sample.diagnosticFlags = static_cast<uint16_t>(source.diagnosticFlags | diagnostics);
    features.baselinePf = baseline;
    features.referenceDeltaPf = 0.0f;
    features.temperatureC = 0.0f;
    features.noisePf = noise;
    features.driftPf = drift;
    features.hasReference = false;
    features.hasTemperature = false;

    // Roadmap v6 safety: feature helpers must not manufacture valid deltas from
    // stale or failed measurements. Profiles still validate status/model again.
    const bool finiteInputs = fdcxIsFiniteFloat(source.capacitancePf) && fdcxIsFiniteFloat(baseline) &&
                              fdcxIsFiniteFloat(noise) && fdcxIsFiniteFloat(drift) && noise >= 0.0f;
    if (source.status != FDCxStatus::Ok || !source.hasCapacitance || !finiteInputs) {
      features.baselineLearned = false;
      features.deltaPf = 0.0f;
      features.normalizedDelta = 0.0f;
      features.compensatedDeltaPf = 0.0f;
      return features;
    }

    features.deltaPf = source.capacitancePf - baseline;
    const float absBaseline = fdcxAbsFloat(baseline);
    const float kNearZeroBaselineEpsilon = 0.001f;
    if (absBaseline < kNearZeroBaselineEpsilon) {
      features.normalizedDelta = 0.0f;
      fdcxSetDiagnosticFlag(features.sample.diagnosticFlags, FDCxDiagnosticFlag::BaselineNearZero);
    } else {
      const float denom = absBaseline + kNearZeroBaselineEpsilon;
      features.normalizedDelta = features.deltaPf / denom;
    }
    features.compensatedDeltaPf = features.deltaPf;
    features.baselineLearned = learned;
    return features;
  }
};

inline bool fdcxFeatureHasUsableCapacitance(const FDCxFeatureVector& input) {
  return fdcxSampleHasKnownModel(input.sample) && input.sample.hasCapacitance;
}
