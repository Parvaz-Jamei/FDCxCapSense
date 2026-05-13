#pragma once

#include <float.h>

inline bool fdcxIsFiniteFloat(float value) {
  return value == value && value <= FLT_MAX && value >= -FLT_MAX;
}

inline float fdcxAbsFloat(float value) {
  return value < 0.0f ? -value : value;
}

inline float fdcxClamp01Finite(float value) {
  if (!fdcxIsFiniteFloat(value)) return 0.0f;
  if (value < 0.0f) return 0.0f;
  if (value > 1.0f) return 1.0f;
  return value;
}
