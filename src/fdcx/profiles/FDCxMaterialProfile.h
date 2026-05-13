#pragma once

#include "ProfileInput.h"
#include "../FDCxStatus.h"

struct FDCxMaterialSignature {
  float normalizedDelta = 0.0f;
  float referenceRatio = 0.0f;
  float stabilityScore = 0.0f;
};

FDCxStatus computeMaterialSignature(const FDCxFeatureVector& input,
                                    FDCxMaterialSignature& output);

#ifndef FDCX_DISABLE_LEGACY_PROFILE_HELPERS
// Legacy/helper API kept for compatibility only. Official examples must use FDCxFeatureVector.
FDCxStatus computeMaterialSignature(float samplePf,
                                    float dryBaselinePf,
                                    float referencePf,
                                    float referenceBaselinePf,
                                    FDCxMaterialSignature& output);
#endif
