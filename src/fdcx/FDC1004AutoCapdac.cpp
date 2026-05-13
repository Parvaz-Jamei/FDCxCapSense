#include "FDC1004.h"
#include "../core/FDCxMath.h"
#include <float.h>

namespace {
constexpr int32_t kRawPositiveNearLimit = 0x006F0000L;
constexpr int32_t kRawNegativeNearLimit = -0x00700000L;
constexpr int32_t kRawPositiveSaturated = 0x007F0000L;
constexpr int32_t kRawNegativeSaturated = -0x00800000L;


FDC1004CapdacReason reasonFromRaw(int32_t raw24) {
  if (raw24 >= kRawPositiveSaturated) {
    return FDC1004CapdacReason::SaturatedHigh;
  }
  if (raw24 <= kRawNegativeSaturated) {
    return FDC1004CapdacReason::SaturatedLow;
  }
  if (raw24 >= kRawPositiveNearLimit) {
    return FDC1004CapdacReason::NearUpperLimit;
  }
  if (raw24 <= kRawNegativeNearLimit) {
    return FDC1004CapdacReason::NearLowerLimit;
  }
  return FDC1004CapdacReason::Ok;
}

bool isSaturatedReason(FDC1004CapdacReason reason) {
  return reason == FDC1004CapdacReason::SaturatedHigh || reason == FDC1004CapdacReason::SaturatedLow;
}

FDC1004CapdacReason statusReason(FDCxStatus status) {
  if (status == FDCxStatus::Timeout) {
    return FDC1004CapdacReason::Timeout;
  }
  if (status == FDCxStatus::InvalidArgument) {
    return FDC1004CapdacReason::InvalidArgument;
  }
  return FDC1004CapdacReason::I2cError;
}
}  // namespace

const char* fdc1004CapdacReasonToString(FDC1004CapdacReason reason) {
  switch (reason) {
    case FDC1004CapdacReason::Ok:
      return "Ok";
    case FDC1004CapdacReason::NearLowerLimit:
      return "NearLowerLimit";
    case FDC1004CapdacReason::NearUpperLimit:
      return "NearUpperLimit";
    case FDC1004CapdacReason::SaturatedLow:
      return "SaturatedLow";
    case FDC1004CapdacReason::SaturatedHigh:
      return "SaturatedHigh";
    case FDC1004CapdacReason::I2cError:
      return "I2cError";
    case FDC1004CapdacReason::Timeout:
      return "Timeout";
    case FDC1004CapdacReason::InvalidArgument:
      return "InvalidArgument";
  }
  return "Unknown";
}

FDC1004AutoCapdacResult FDC1004::autoTuneCapdac(FDC1004Measurement measurement,
                                                FDC1004Channel channel,
                                                uint8_t minCode,
                                                uint8_t maxCode,
                                                float targetCenterPf,
                                                float marginPf) {
  FDC1004AutoCapdacResult result;
  result.status = FDCxStatus::InvalidArgument;
  result.reason = FDC1004CapdacReason::InvalidArgument;

  if (!isValidMeasurement(measurement) || !isValidChannel(channel) ||
      minCode > maxCode || maxCode > fdc1004_registers::kMaxCapdacCode ||
      !fdcxIsFiniteFloat(targetCenterPf) || !fdcxIsFiniteFloat(marginPf) || marginPf < 0.0f) {
    setStatus(FDCxStatus::InvalidArgument);
    return result;
  }

  float bestError = FLT_MAX;
  bool haveCandidate = false;
  FDC1004AutoCapdacResult lastSaturated;
  lastSaturated.status = FDCxStatus::MeasurementNotReady;

  for (uint8_t code = minCode; code <= maxCode; ++code) {
    FDCxStatus status = configureSingleEndedWithCapdac(measurement, channel, code);
    if (status != FDCxStatus::Ok) {
      result.status = status;
      result.reason = statusReason(status);
      setStatus(status);
      return result;
    }

    int32_t raw = 0;
    float pf = 0.0f;
    status = readSingleMeasurement(measurement, raw, pf);
    if (status != FDCxStatus::Ok) {
      result.status = status;
      result.reason = statusReason(status);
      setStatus(status);
      return result;
    }

    const float residualPf = static_cast<float>(raw) / fdc1004_registers::kRawDenominator;
    const FDC1004CapdacReason rawReason = reasonFromRaw(raw);
    const bool saturated = isSaturatedReason(rawReason);
    const float residualError = fdcxAbsFloat(residualPf - targetCenterPf);

    FDC1004AutoCapdacResult candidate;
    candidate.status = saturated ? FDCxStatus::MeasurementNotReady : FDCxStatus::Ok;
    candidate.capdacCode = code;
    candidate.capacitancePf = pf;
    candidate.residualPf = residualPf;
    candidate.raw24 = raw;
    candidate.reason = rawReason;
    candidate.saturated = saturated;

    if (saturated) {
      lastSaturated = candidate;
    } else {
      if (!haveCandidate || residualError < bestError) {
        haveCandidate = true;
        bestError = residualError;
        result = candidate;
      }
      if (residualError <= marginPf) {
        const FDCxStatus finalStatus = configureSingleEndedWithCapdac(measurement, channel, code);
        if (finalStatus != FDCxStatus::Ok) {
          candidate.status = finalStatus;
          candidate.reason = statusReason(finalStatus);
          setStatus(finalStatus);
          return candidate;
        }
        setStatus(FDCxStatus::Ok);
        return candidate;
      }
    }

    if (code == 255u) {
      break;
    }
  }

  if (haveCandidate) {
    const FDCxStatus finalStatus = configureSingleEndedWithCapdac(measurement, channel, result.capdacCode);
    if (finalStatus != FDCxStatus::Ok) {
      result.status = finalStatus;
      result.reason = statusReason(finalStatus);
      setStatus(finalStatus);
      return result;
    }
    setStatus(FDCxStatus::Ok);
    return result;
  }

  setStatus(FDCxStatus::MeasurementNotReady);
  return lastSaturated;
}
