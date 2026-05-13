#pragma once

#include <stdint.h>

enum class FDCxMeasurementModel : uint8_t {
  Unknown = 0,
  DirectCapacitance,
  ResonantFrequency,
  DerivedCapacitance
};

inline const char* fdcxMeasurementModelToString(FDCxMeasurementModel model) {
  switch (model) {
    case FDCxMeasurementModel::Unknown: return "Unknown";
    case FDCxMeasurementModel::DirectCapacitance: return "DirectCapacitance";
    case FDCxMeasurementModel::ResonantFrequency: return "ResonantFrequency";
    case FDCxMeasurementModel::DerivedCapacitance: return "DerivedCapacitance";
  }
  return "Unknown";
}
