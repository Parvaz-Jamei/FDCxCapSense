#pragma once
#include <stdint.h>

enum class FDC2x1xVariant : uint8_t {
  FDC2112 = 0,
  FDC2114 = 1,
  FDC2212 = 2,
  FDC2214 = 3
};

enum class FDC2x1xChannel : uint8_t {
  CH0 = 0,
  CH1 = 1,
  CH2 = 2,
  CH3 = 3
};

enum class FDC2x1xDeglitch : uint8_t {
  MHz_1 = 1,
  MHz_3_3 = 4,
  MHz_10 = 5,
  MHz_33 = 7
};


enum class FDC2x1xReferenceClockSource : uint8_t {
  InternalOscillator = 0,
  ExternalClockInput = 1
};

enum class FDC2x1xSensorTopology : uint8_t {
  SingleEnded = 0,
  Differential = 1
};

inline bool fdc2x1xVariantIsKnown(FDC2x1xVariant variant) {
  return variant == FDC2x1xVariant::FDC2112 ||
         variant == FDC2x1xVariant::FDC2114 ||
         variant == FDC2x1xVariant::FDC2212 ||
         variant == FDC2x1xVariant::FDC2214;
}

inline bool fdc2x1xVariantIsFdc211x(FDC2x1xVariant variant) {
  return variant == FDC2x1xVariant::FDC2112 || variant == FDC2x1xVariant::FDC2114;
}

inline bool fdc2x1xVariantIsFdc221x(FDC2x1xVariant variant) {
  return variant == FDC2x1xVariant::FDC2212 || variant == FDC2x1xVariant::FDC2214;
}

inline uint8_t fdc2x1xVariantChannelCount(FDC2x1xVariant variant) {
  if (!fdc2x1xVariantIsKnown(variant)) return 0u;
  return (variant == FDC2x1xVariant::FDC2112 || variant == FDC2x1xVariant::FDC2212) ? 2u : 4u;
}

inline bool fdc2x1xChannelIsValid(FDC2x1xVariant variant, FDC2x1xChannel channel) {
  if (!fdc2x1xVariantIsKnown(variant)) return false;
  return static_cast<uint8_t>(channel) < fdc2x1xVariantChannelCount(variant);
}
