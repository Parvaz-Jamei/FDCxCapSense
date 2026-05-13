#pragma once

#include "FDCxProfileTypes.h"
#include "FDCxConfidence.h"
#include "../../core/FDCxFeatureVector.h"
#include "../FDCxDiagnostics.h"

inline FDCxProfileResult fdcxRejectProfileInput(const char* reason,
                                                FDCxStatus status = FDCxStatus::InvalidArgument,
                                                FDCxProfileState state = FDCxProfileState::Invalid) {
  FDCxProfileResult result;
  result.status = status;
  result.state = state;
  result.confidence = 0.0f;
  result.reason = reason;
  return result;
}

inline bool fdcxProfileInputHasKnownFamilyAndModel(const FDCxFeatureVector& input) {
  return input.sample.family != FDCxFamily::Unknown && input.sample.model != FDCxMeasurementModel::Unknown;
}

inline bool fdcxProfileInputHasCapacitance(const FDCxFeatureVector& input) {
  return fdcxProfileInputHasKnownFamilyAndModel(input) && input.sample.hasCapacitance;
}

inline bool fdcxProfileInputStatusOk(const FDCxFeatureVector& input) {
  return input.sample.status == FDCxStatus::Ok;
}

inline bool fdcxProfileInputIsDirectFDC1004Capacitance(const FDCxFeatureVector& input) {
  return input.sample.family == FDCxFamily::FDC1004_DirectCDC &&
         input.sample.model == FDCxMeasurementModel::DirectCapacitance &&
         input.sample.hasCapacitance &&
         input.sample.capacitanceIsDirect &&
         !input.sample.capacitanceIsDerived;
}

inline FDCxProfileResult fdcxRejectIfNotDirectFDC1004(const FDCxFeatureVector& input) {
  if (!fdcxProfileInputStatusOk(input)) {
    return fdcxRejectProfileInput("InputSampleNotOk", input.sample.status, FDCxProfileState::Unknown);
  }
  if (!fdcxProfileInputIsDirectFDC1004Capacitance(input)) {
    return fdcxRejectProfileInput("DirectFDC1004CapacitanceRequired");
  }
  FDCxProfileResult accepted;
  accepted.status = FDCxStatus::Ok;
  accepted.state = FDCxProfileState::Unknown;
  accepted.reason = "Accepted";
  return accepted;
}


inline FDCxProfileResult fdcxRejectIfInvalidNoise(const FDCxFeatureVector& input) {
  if (!fdcxProfileIsFinite(input.noisePf) || input.noisePf < 0.0f) {
    return fdcxRejectProfileInput("InvalidNoise");
  }
  FDCxProfileResult accepted;
  accepted.status = FDCxStatus::Ok;
  accepted.state = FDCxProfileState::Unknown;
  accepted.reason = "Accepted";
  return accepted;
}

inline bool fdcxProfileBaselineReady(const FDCxFeatureVector& input) {
  return input.baselineLearned && !fdcxHasDiagnosticFlag(input.sample.diagnosticFlags, FDCxDiagnosticFlag::BaselineNotLearned);
}

inline FDCxProfileMetrics fdcxMetricsFromFeatures(const FDCxFeatureVector& input) {
  FDCxProfileMetrics metrics;
  metrics.rawPf = input.sample.capacitancePf;
  metrics.baselinePf = input.baselinePf;
  metrics.deltaPf = input.deltaPf;
  metrics.compensatedDeltaPf = input.compensatedDeltaPf;
  metrics.referenceDeltaPf = input.referenceDeltaPf;
  metrics.temperatureC = input.temperatureC;
  metrics.noisePf = input.noisePf;
  metrics.driftPf = input.driftPf;
  metrics.diagnosticFlags = input.sample.diagnosticFlags;
  return metrics;
}
