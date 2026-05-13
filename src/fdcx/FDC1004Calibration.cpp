#include "FDC1004Calibration.h"
#include "../core/FDCxMath.h"

FDCxReferenceCompensation compensateWithReference(float primaryPf,
                                                  float primaryBaselinePf,
                                                  float referencePf,
                                                  float referenceBaselinePf,
                                                  float referenceScale) {
  FDCxReferenceCompensation result;
  result.primaryDeltaPf = primaryPf - primaryBaselinePf;
  result.referenceDeltaPf = referencePf - referenceBaselinePf;
  result.compensatedDeltaPf = result.primaryDeltaPf - (result.referenceDeltaPf * referenceScale);
  return result;
}

FDCxStatus compensateWithReferenceChecked(float primaryPf,
                                          float primaryBaselinePf,
                                          float referencePf,
                                          float referenceBaselinePf,
                                          float referenceScale,
                                          FDCxReferenceCompensation& output) {
  if (!fdcxIsFiniteFloat(primaryPf) || !fdcxIsFiniteFloat(primaryBaselinePf) ||
      !fdcxIsFiniteFloat(referencePf) || !fdcxIsFiniteFloat(referenceBaselinePf) ||
      !fdcxIsFiniteFloat(referenceScale)) {
    output = FDCxReferenceCompensation();
    return FDCxStatus::InvalidArgument;
  }
  output = compensateWithReference(primaryPf, primaryBaselinePf, referencePf, referenceBaselinePf, referenceScale);
  return FDCxStatus::Ok;
}
