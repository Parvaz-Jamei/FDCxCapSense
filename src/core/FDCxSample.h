#pragma once

#include <stdint.h>
#include "FDCxFamily.h"
#include "FDCxMeasurementModel.h"
#include "FDCxMath.h"
#include "../fdcx/FDCxStatus.h"

struct FDCxSample {
  FDCxFamily family = FDCxFamily::Unknown;
  FDCxMeasurementModel model = FDCxMeasurementModel::Unknown;
  uint8_t channel = 0;
  float capacitancePf = 0.0f;
  float frequencyHz = 0.0f;
  bool hasCapacitance = false;
  bool hasFrequency = false;
  bool capacitanceIsDirect = false;
  bool capacitanceIsDerived = false;
  float confidence = 0.0f;
  FDCxStatus status = FDCxStatus::InvalidArgument;
  uint16_t diagnosticFlags = 0;

  static FDCxSample directFDC1004(uint8_t channelIndex,
                                  float capacitance,
                                  FDCxStatus readStatus = FDCxStatus::Ok,
                                  uint16_t diagnostics = 0,
                                  float sampleConfidence = 1.0f) {
    FDCxSample sample;
    sample.family = FDCxFamily::FDC1004_DirectCDC;
    sample.model = FDCxMeasurementModel::DirectCapacitance;
    const bool ok = (readStatus == FDCxStatus::Ok);
    sample.channel = channelIndex;
    sample.capacitancePf = ok ? capacitance : 0.0f;
    sample.frequencyHz = 0.0f;
    sample.hasCapacitance = ok;
    sample.hasFrequency = false;
    sample.capacitanceIsDirect = ok;
    sample.capacitanceIsDerived = false;
    sample.confidence = ok ? fdcxClamp01Finite(sampleConfidence) : 0.0f;
    sample.status = readStatus;
    sample.diagnosticFlags = diagnostics;
    return sample;
  }
};

inline bool fdcxSampleHasKnownModel(const FDCxSample& sample) {
  return sample.family != FDCxFamily::Unknown && sample.model != FDCxMeasurementModel::Unknown;
}

inline bool fdcxSampleHasDirectCapacitance(const FDCxSample& sample) {
  return fdcxSampleHasKnownModel(sample) && sample.hasCapacitance &&
         sample.model == FDCxMeasurementModel::DirectCapacitance && sample.capacitanceIsDirect &&
         !sample.capacitanceIsDerived;
}
