#pragma once

#include <stdint.h>

struct FDCxCapability {
  bool supportsDirectCapacitance = false;
  bool supportsResonantFrequency = false;
  bool chipUsesResonantFrequency = false;
  bool driverSupportsFrequencyConversion = false;
  bool supportsCapdac = false;
  bool supportsRepeatMode = false;
  bool supportsTemperatureInput = false;

  // Chip-level capability vs driver-level implementation are intentionally separate.
  bool chipHasIntbDrdy = false;
  bool driverSupportsInterruptCallbacks = false;

  // Compatibility field: true only when this driver exposes interrupt/callback handling.
  bool supportsHardwareInterrupt = false;
};
