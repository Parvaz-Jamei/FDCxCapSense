#pragma once

struct FDCxHysteresis {
  float risingThreshold = 1.0f;
  float fallingThreshold = 0.5f;
  bool state = false;

  bool update(float value) {
    if (!state && value >= risingThreshold) state = true;
    else if (state && value <= fallingThreshold) state = false;
    return state;
  }
};
