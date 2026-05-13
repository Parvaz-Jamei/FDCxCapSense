#pragma once

struct FDC2x1xLCParams {
  float inductanceUH = 0.0f;
  float parasiticCapacitancePf = 0.0f;

  // Optional trace metadata. Derived capacitance uses frequency.frequencyHz, inductanceUH,
  // and parasiticCapacitancePf. The effective reference clock is passed to readFrequencyHz().
  float traceReferenceClockHz = 0.0f;
};
