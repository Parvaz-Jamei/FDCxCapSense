#pragma once

#include <stdint.h>

enum class FDCxFamily : uint8_t {
  Unknown = 0,
  FDC1004_DirectCDC,
  FDC2x1x_ResonantLC
};

inline const char* fdcxFamilyToString(FDCxFamily family) {
  switch (family) {
    case FDCxFamily::Unknown: return "Unknown";
    case FDCxFamily::FDC1004_DirectCDC: return "FDC1004_DirectCDC";
    case FDCxFamily::FDC2x1x_ResonantLC: return "FDC2x1x_ResonantLC";
  }
  return "Unknown";
}
