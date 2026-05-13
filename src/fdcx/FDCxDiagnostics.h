#pragma once

#include <stdint.h>

enum class FDCxDiagnosticFlag : uint16_t {
  None = 0,
  I2cError = static_cast<uint16_t>(1u << 0),
  Timeout = static_cast<uint16_t>(1u << 1),
  RawNearPositiveLimit = static_cast<uint16_t>(1u << 2),
  RawNearNegativeLimit = static_cast<uint16_t>(1u << 3),
  CapdacAtMinimum = static_cast<uint16_t>(1u << 4),
  CapdacAtMaximum = static_cast<uint16_t>(1u << 5),
  BaselineNotLearned = static_cast<uint16_t>(1u << 6),
  SignalUnstable = static_cast<uint16_t>(1u << 7),
  InvalidInput = static_cast<uint16_t>(1u << 8),
  BaselineNearZero = static_cast<uint16_t>(1u << 9)
};

struct FDCxDiagnosticReport {
  uint16_t flags = 0;
  float capacitancePf = 0.0f;
  float baselinePf = 0.0f;
  float deltaPf = 0.0f;
  float noisePf = 0.0f;
  const char* reason = "Ok";
};

uint16_t fdcxFlag(FDCxDiagnosticFlag flag);
bool fdcxHasDiagnosticFlag(uint16_t flags, FDCxDiagnosticFlag flag);
void fdcxSetDiagnosticFlag(uint16_t& flags, FDCxDiagnosticFlag flag);

FDCxDiagnosticReport fdcxBuildDiagnosticReport(float capacitancePf,
                                               float baselinePf,
                                               float noisePf,
                                               bool baselineLearned,
                                               uint8_t capdacCode,
                                               int32_t raw24,
                                               float rawNearLimitRatio = 0.85f,
                                               float unstableNoisePf = 0.10f,
                                               bool capdacEnabled = false);
