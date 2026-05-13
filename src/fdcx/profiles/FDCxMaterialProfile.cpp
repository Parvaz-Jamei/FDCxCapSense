#include "FDCxMaterialProfile.h"
#include "FDCxConfidence.h"

FDCxStatus computeMaterialSignature(const FDCxFeatureVector& input,
                                    FDCxMaterialSignature& output) {
  output = FDCxMaterialSignature();
  if (!fdcxProfileInputStatusOk(input)) {
    return input.sample.status;
  }
  if (!fdcxProfileInputIsDirectFDC1004Capacitance(input)) {
    return FDCxStatus::InvalidArgument;
  }
  if (!input.baselineLearned || !fdcxProfileIsFinite(input.sample.capacitancePf) || !fdcxProfileIsFinite(input.baselinePf) ||
      !fdcxProfileIsFinite(input.noisePf) || input.noisePf < 0.0f || !fdcxProfileIsFinite(input.deltaPf)) {
    return FDCxStatus::InvalidArgument;
  }
  const float denom = fdcxProfileAbs(input.baselinePf) + 0.001f;
  output.normalizedDelta = input.deltaPf / denom;
  if (input.hasReference && fdcxProfileIsFinite(input.referenceDeltaPf) && input.referenceDeltaPf != 0.0f) {
    output.referenceRatio = input.deltaPf / input.referenceDeltaPf;
    output.stabilityScore = fdcxClamp01(1.0f - fdcxProfileAbs(input.referenceDeltaPf) /
                                                (fdcxProfileAbs(input.deltaPf) + 0.001f));
  } else {
    output.referenceRatio = 0.0f;
    output.stabilityScore = fdcxClamp01(1.0f - input.noisePf / (fdcxProfileAbs(input.deltaPf) + 0.001f));
  }
  return FDCxStatus::Ok;
}

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
FDCxStatus computeMaterialSignature(float samplePf,
                                    float dryBaselinePf,
                                    float referencePf,
                                    float referenceBaselinePf,
                                    FDCxMaterialSignature& output) {
  FDCxSample sample = FDCxSample::directFDC1004(0, samplePf, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector input = FDCxFeatureVector::fromSample(sample, dryBaselinePf, 0.0f, 0.0f, 0, true);
  input.hasReference = true;
  input.referenceDeltaPf = referencePf - referenceBaselinePf;
  return computeMaterialSignature(input, output);
}
#endif
