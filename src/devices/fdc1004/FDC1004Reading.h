// Compatibility forwarding header for the transitional Roadmap v6 layout.
// Implementation currently remains in src/fdcx until the Phase 4 refactor.
#pragma once

#include <stdint.h>
#include "../../core/FDCxSample.h"

struct FDC1004Reading {
  uint8_t channel = 0;
  int32_t raw24 = 0;
  float capacitancePf = 0.0f;
  uint8_t capdacCode = 0;
  bool capdacEnabled = false;
  FDCxStatus status = FDCxStatus::InvalidArgument;
  uint16_t diagnosticFlags = 0;

  FDCxSample toSample(float confidence = 1.0f) const {
    return FDCxSample::directFDC1004(channel, capacitancePf, status, diagnosticFlags, confidence);
  }
};
