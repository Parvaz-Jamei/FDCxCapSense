#pragma once

#include <stdint.h>
#include "../FDCxStatus.h"

enum class FDCxProfileState : uint8_t {
  Unknown = 0,
  Stable,
  BelowThreshold,
  AboveThreshold,
  Near,
  Far,
  Empty,
  Full,
  Partial,
  Dry,
  WetCandidate,
  FrostCandidate,
  IceCandidate,
  MaterialChanged,
  Unstable,
  Candidate,
  Invalid
};

struct FDCxProfileMetrics {
  float rawPf = 0.0f;
  float baselinePf = 0.0f;
  float deltaPf = 0.0f;
  float compensatedDeltaPf = 0.0f;
  float referenceDeltaPf = 0.0f;
  float temperatureC = 0.0f;
  float noisePf = 0.0f;
  float driftPf = 0.0f;
  uint8_t capdacCode = 0;
  uint16_t diagnosticFlags = 0;
};

struct FDCxProfileResult {
  FDCxStatus status = FDCxStatus::InvalidArgument;
  FDCxProfileState state = FDCxProfileState::Unknown;
  float confidence = 0.0f;
  const char* reason = "NotEvaluated";
  FDCxProfileMetrics metrics;
};

const char* fdcxProfileStateToString(FDCxProfileState state);
