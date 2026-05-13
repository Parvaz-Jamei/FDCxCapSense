#pragma once

struct FDCxDerivativeState {
  float previous = 0.0f;
  bool hasPrevious = false;
};

inline float fdcxDerivative(FDCxDerivativeState& state, float current) {
  if (!state.hasPrevious) {
    state.previous = current;
    state.hasPrevious = true;
    return 0.0f;
  }
  const float delta = current - state.previous;
  state.previous = current;
  return delta;
}
