#pragma once

#include "FDCxConfidence.h"

inline bool fdcxAboveThreshold(float value, float threshold) {
  return fdcxProfileIsFinite(value) && fdcxProfileIsFinite(threshold) && value >= threshold;
}

inline bool fdcxBelowThreshold(float value, float threshold) {
  return fdcxProfileIsFinite(value) && fdcxProfileIsFinite(threshold) && value <= threshold;
}
