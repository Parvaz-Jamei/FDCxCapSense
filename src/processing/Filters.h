#pragma once

inline float fdcxExponentialMovingAverage(float previous, float current, float alpha) {
  if (alpha < 0.0f) alpha = 0.0f;
  if (alpha > 1.0f) alpha = 1.0f;
  return previous + (alpha * (current - previous));
}
