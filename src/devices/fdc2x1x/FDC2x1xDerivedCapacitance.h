#pragma once
#include "FDC2x1xTypes.h"
#include "../../fdcx/FDCxStatus.h"
#include "../../core/FDCxSample.h"
#include "../../core/FDCxMath.h"

struct FDC2x1xDerivedCapacitance {
  FDCxStatus status = FDCxStatus::InvalidArgument;
  FDC2x1xVariant variant = FDC2x1xVariant::FDC2214;
  FDC2x1xChannel channel = FDC2x1xChannel::CH0;
  float capacitancePf = 0.0f;
  bool isDerived = true;
  const char* modelWarning = "Derived capacitance is LC-model-dependent";

  FDCxSample toSample(float confidence = 1.0f) const {
    FDCxSample sample;
    sample.family = FDCxFamily::FDC2x1x_ResonantLC;
    sample.model = FDCxMeasurementModel::DerivedCapacitance;
    sample.channel = static_cast<uint8_t>(channel);
    sample.capacitancePf = (status == FDCxStatus::Ok) ? capacitancePf : 0.0f;
    sample.hasCapacitance = (status == FDCxStatus::Ok);
    sample.hasFrequency = false;
    sample.capacitanceIsDirect = false;
    sample.capacitanceIsDerived = (status == FDCxStatus::Ok);
    sample.confidence = fdcxClamp01Finite(confidence);
    sample.status = status;
    return sample;
  }
};

inline FDCxSample toSample(const FDC2x1xDerivedCapacitance& reading, float confidence = 1.0f) {
  return reading.toSample(confidence);
}
