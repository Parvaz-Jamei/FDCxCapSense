#pragma once

#include "FDCxStatus.h"

struct FDCxReferenceCompensation {
  float primaryDeltaPf = 0.0f;
  float referenceDeltaPf = 0.0f;
  float compensatedDeltaPf = 0.0f;
};

FDCxReferenceCompensation compensateWithReference(float primaryPf,
                                                  float primaryBaselinePf,
                                                  float referencePf,
                                                  float referenceBaselinePf,
                                                  float referenceScale);

FDCxStatus compensateWithReferenceChecked(float primaryPf,
                                          float primaryBaselinePf,
                                          float referencePf,
                                          float referenceBaselinePf,
                                          float referenceScale,
                                          FDCxReferenceCompensation& output);
