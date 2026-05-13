#pragma once

#include <stdint.h>

enum class FDCxChipId : uint8_t {
  Unknown = 0,
  FDC1004,
  FDC1004_Q1,
  FDC2112,
  FDC2114,
  FDC2212,
  FDC2214,
  FDC2x1x_Future
};
