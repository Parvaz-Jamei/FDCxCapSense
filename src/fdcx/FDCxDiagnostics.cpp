#include "FDCxDiagnostics.h"
#include "../core/FDCxMath.h"
#include "FDC1004Registers.h"

namespace {

const char* reasonFromFlags(uint16_t flags) {
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::InvalidInput)) {
    return "InvalidInput";
  }
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::BaselineNearZero)) {
    return "BaselineNearZero";
  }
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::BaselineNotLearned)) {
    return "BaselineNotLearned";
  }
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::RawNearPositiveLimit)) {
    return "RawNearPositiveLimit";
  }
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::RawNearNegativeLimit)) {
    return "RawNearNegativeLimit";
  }
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::SignalUnstable)) {
    return "SignalUnstable";
  }
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::CapdacAtMinimum)) {
    return "CapdacAtMinimum";
  }
  if (fdcxHasDiagnosticFlag(flags, FDCxDiagnosticFlag::CapdacAtMaximum)) {
    return "CapdacAtMaximum";
  }
  return "Ok";
}
}  // namespace

uint16_t fdcxFlag(FDCxDiagnosticFlag flag) {
  return static_cast<uint16_t>(flag);
}

bool fdcxHasDiagnosticFlag(uint16_t flags, FDCxDiagnosticFlag flag) {
  return (flags & fdcxFlag(flag)) != 0u;
}

void fdcxSetDiagnosticFlag(uint16_t& flags, FDCxDiagnosticFlag flag) {
  flags = static_cast<uint16_t>(flags | fdcxFlag(flag));
}

FDCxDiagnosticReport fdcxBuildDiagnosticReport(float capacitancePf,
                                               float baselinePf,
                                               float noisePf,
                                               bool baselineLearned,
                                               uint8_t capdacCode,
                                               int32_t raw24,
                                               float rawNearLimitRatio,
                                               float unstableNoisePf,
                                               bool capdacEnabled) {
  FDCxDiagnosticReport report;
  report.capacitancePf = capacitancePf;
  report.baselinePf = baselinePf;
  report.deltaPf = capacitancePf - baselinePf;
  report.noisePf = noisePf;

  if (!fdcxIsFiniteFloat(capacitancePf) || !fdcxIsFiniteFloat(baselinePf) ||
      !fdcxIsFiniteFloat(noisePf) || !fdcxIsFiniteFloat(rawNearLimitRatio) ||
      !fdcxIsFiniteFloat(unstableNoisePf) || noisePf < 0.0f || unstableNoisePf < 0.0f ||
      rawNearLimitRatio <= 0.0f || rawNearLimitRatio > 1.0f) {
    fdcxSetDiagnosticFlag(report.flags, FDCxDiagnosticFlag::InvalidInput);
  }
  if (!baselineLearned) {
    fdcxSetDiagnosticFlag(report.flags, FDCxDiagnosticFlag::BaselineNotLearned);
  }
  if (noisePf > unstableNoisePf) {
    fdcxSetDiagnosticFlag(report.flags, FDCxDiagnosticFlag::SignalUnstable);
  }
  if (capdacEnabled && capdacCode == 0u) {
    fdcxSetDiagnosticFlag(report.flags, FDCxDiagnosticFlag::CapdacAtMinimum);
  }
  if (capdacEnabled && capdacCode >= fdc1004_registers::kMaxCapdacCode) {
    fdcxSetDiagnosticFlag(report.flags, FDCxDiagnosticFlag::CapdacAtMaximum);
  }

  const float positiveLimit = 8388607.0f * rawNearLimitRatio;
  const float negativeLimit = -8388608.0f * rawNearLimitRatio;
  if (static_cast<float>(raw24) >= positiveLimit) {
    fdcxSetDiagnosticFlag(report.flags, FDCxDiagnosticFlag::RawNearPositiveLimit);
  }
  if (static_cast<float>(raw24) <= negativeLimit) {
    fdcxSetDiagnosticFlag(report.flags, FDCxDiagnosticFlag::RawNearNegativeLimit);
  }

  report.reason = reasonFromFlags(report.flags);
  return report;
}
