#pragma once

#include <stdint.h>

struct FDCxBaselineConfig {
  FDCxBaselineConfig() = default;
  FDCxBaselineConfig(uint16_t warmup, uint16_t minStable, float maxNoise, float learning, float drift)
      : warmupSamples(warmup),
        minStableSamples(minStable),
        maxNoisePf(maxNoise),
        learningRate(learning),
        driftLimitPf(drift) {}

  uint16_t warmupSamples = 8;
  uint16_t minStableSamples = 8;
  float maxNoisePf = 0.05f;
  float learningRate = 0.05f;
  float driftLimitPf = 2.0f;
};

struct FDCxBaselineState {
  bool learned = false;
  uint32_t sampleCount = 0;
  float baselinePf = 0.0f;
  float noisePf = 0.0f;
  float driftPf = 0.0f;
};

class FDCxBaselineTracker {
 public:
  explicit FDCxBaselineTracker(const FDCxBaselineConfig& config);

  void reset();
  bool update(float capacitancePf);
  FDCxBaselineState state() const;
  float deltaFromBaseline(float capacitancePf) const;

 private:
  static bool isFinite(float value);
  static float absFloat(float value);
  static float clampLearningRate(float value);

  FDCxBaselineConfig _config;
  FDCxBaselineState _state;
  uint16_t _stableSamples = 0;
};
