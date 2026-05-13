#pragma once

#include <float.h>

inline bool fdcxProfileIsFinite(float value) {
  return value == value && value <= FLT_MAX && value >= -FLT_MAX;
}

inline float fdcxProfileAbs(float value) {
  return value < 0.0f ? -value : value;
}

inline float fdcxClamp01(float value) {
  if (!fdcxProfileIsFinite(value)) {
    return 0.0f;
  }
  if (value < 0.0f) {
    return 0.0f;
  }
  if (value > 1.0f) {
    return 1.0f;
  }
  return value;
}

inline float fdcxConfidenceFromMargin(float signalPf, float thresholdPf, float noisePf) {
  if (!fdcxProfileIsFinite(signalPf) || !fdcxProfileIsFinite(thresholdPf) || !fdcxProfileIsFinite(noisePf) || thresholdPf <= 0.0f) {
    return 0.0f;
  }
  const float normalized = fdcxProfileAbs(signalPf) / thresholdPf;
  const float noisePenalty = noisePf > 0.0f ? noisePf / thresholdPf : 0.0f;
  return fdcxClamp01(normalized - noisePenalty);
}
