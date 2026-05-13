#pragma once

#include <stdint.h>
#include "FDCxStatus.h"

enum class FDC1004CapdacReason : uint8_t {
  Ok = 0,
  NearLowerLimit,
  NearUpperLimit,
  SaturatedLow,
  SaturatedHigh,
  I2cError,
  Timeout,
  InvalidArgument
};

struct FDC1004AutoCapdacResult {
  FDCxStatus status = FDCxStatus::InvalidArgument;
  uint8_t capdacCode = 0;
  float capacitancePf = 0.0f;
  float residualPf = 0.0f;
  int32_t raw24 = 0;
  FDC1004CapdacReason reason = FDC1004CapdacReason::InvalidArgument;
  bool saturated = false;
};

const char* fdc1004CapdacReasonToString(FDC1004CapdacReason reason);
