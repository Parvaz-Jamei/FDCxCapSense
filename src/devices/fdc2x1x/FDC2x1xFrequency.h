#pragma once
#include "FDC2x1xTypes.h"
#include "../../fdcx/FDCxStatus.h"
#include "../../core/FDCxSample.h"
#include "../../core/FDCxMath.h"

struct FDC2x1xFrequency {
  FDCxStatus status = FDCxStatus::InvalidArgument;
  FDC2x1xVariant variant = FDC2x1xVariant::FDC2214;
  FDC2x1xChannel channel = FDC2x1xChannel::CH0;
  uint32_t rawCode = 0;
  float frequencyHz = 0.0f;
  float frefHz = 0.0f;
  uint8_t finSelMultiplier = 0;
  uint16_t frefDivider = 0;
  bool dataReady = false;

  FDCxSample toSample(float confidence = 1.0f) const {
    FDCxSample sample;
    sample.family = FDCxFamily::FDC2x1x_ResonantLC;
    sample.model = FDCxMeasurementModel::ResonantFrequency;
    sample.channel = static_cast<uint8_t>(channel);
    sample.frequencyHz = (status == FDCxStatus::Ok) ? frequencyHz : 0.0f;
    sample.hasFrequency = (status == FDCxStatus::Ok);
    sample.hasCapacitance = false;
    sample.capacitanceIsDirect = false;
    sample.capacitanceIsDerived = false;
    sample.confidence = fdcxClamp01Finite(confidence);
    sample.status = status;
    return sample;
  }
};

inline FDCxSample toSample(const FDC2x1xFrequency& reading, float confidence = 1.0f) {
  return reading.toSample(confidence);
}
