#include "FDCxTemperatureCompensation.h"
#include "../core/FDCxMath.h"

float compensateTemperatureLinear(float capacitancePf,
                                  float temperatureC,
                                  const FDCxTemperatureCompensationConfig& config) {
  if (!fdcxIsFiniteFloat(capacitancePf) || !fdcxIsFiniteFloat(temperatureC) ||
      !fdcxIsFiniteFloat(config.referenceTemperatureC) || !fdcxIsFiniteFloat(config.alphaPfPerC)) {
    return 0.0f / 0.0f;
  }
  return capacitancePf - (config.alphaPfPerC * (temperatureC - config.referenceTemperatureC));
}

FDCxStatus compensateTemperatureLinearChecked(float capacitancePf,
                                              float temperatureC,
                                              const FDCxTemperatureCompensationConfig& config,
                                              float& compensatedPf) {
  if (!fdcxIsFiniteFloat(capacitancePf) || !fdcxIsFiniteFloat(temperatureC) ||
      !fdcxIsFiniteFloat(config.referenceTemperatureC) || !fdcxIsFiniteFloat(config.alphaPfPerC)) {
    compensatedPf = 0.0f;
    return FDCxStatus::InvalidArgument;
  }
  compensatedPf = compensateTemperatureLinear(capacitancePf, temperatureC, config);
  return FDCxStatus::Ok;
}

FDCxStatus compensateTemperatureLookup(float capacitancePf,
                                       float temperatureC,
                                       const FDCxTemperaturePoint* table,
                                       uint8_t count,
                                       float& compensatedPf) {
  compensatedPf = 0.0f;
  if (!fdcxIsFiniteFloat(capacitancePf) || !fdcxIsFiniteFloat(temperatureC) || table == nullptr || count < 2u) {
    return FDCxStatus::InvalidArgument;
  }
  for (uint8_t i = 0; i < count; ++i) {
    if (!fdcxIsFiniteFloat(table[i].temperatureC) || !fdcxIsFiniteFloat(table[i].offsetPf)) {
      return FDCxStatus::InvalidArgument;
    }
    if (i > 0u && table[i].temperatureC <= table[i - 1u].temperatureC) {
      return FDCxStatus::InvalidArgument;
    }
  }
  if (temperatureC < table[0].temperatureC || temperatureC > table[count - 1u].temperatureC) {
    return FDCxStatus::InvalidArgument;
  }
  for (uint8_t i = 1; i < count; ++i) {
    if (temperatureC <= table[i].temperatureC) {
      const float lowT = table[i - 1u].temperatureC;
      const float highT = table[i].temperatureC;
      const float span = highT - lowT;
      if (span <= 0.0f) {
        return FDCxStatus::InvalidArgument;
      }
      const float ratio = (temperatureC - lowT) / span;
      const float offset = table[i - 1u].offsetPf + ((table[i].offsetPf - table[i - 1u].offsetPf) * ratio);
      compensatedPf = capacitancePf - offset;
      return FDCxStatus::Ok;
    }
  }
  compensatedPf = capacitancePf - table[count - 1u].offsetPf;
  return FDCxStatus::Ok;
}
