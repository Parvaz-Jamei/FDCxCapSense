#pragma once

#include <stdint.h>
#include "FDCxStatus.h"

struct FDCxTemperatureCompensationConfig {
  float referenceTemperatureC = 25.0f;
  float alphaPfPerC = 0.0f;
};

struct FDCxTemperaturePoint {
  FDCxTemperaturePoint() = default;
  FDCxTemperaturePoint(float temperature, float offset) : temperatureC(temperature), offsetPf(offset) {}

  float temperatureC = 0.0f;
  float offsetPf = 0.0f;
};

float compensateTemperatureLinear(float capacitancePf,
                                  float temperatureC,
                                  const FDCxTemperatureCompensationConfig& config);

FDCxStatus compensateTemperatureLinearChecked(float capacitancePf,
                                              float temperatureC,
                                              const FDCxTemperatureCompensationConfig& config,
                                              float& compensatedPf);

FDCxStatus compensateTemperatureLookup(float capacitancePf,
                                       float temperatureC,
                                       const FDCxTemperaturePoint* table,
                                       uint8_t count,
                                       float& compensatedPf);
