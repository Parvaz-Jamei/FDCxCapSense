#pragma once
// Lightweight FDC2x1x-only public include. Frequency-first; no FDC1004 CAPDAC helpers or profiles are included here.
#include "core/FDCStatus.h"
#include "core/FDCI2C.h"
#include "core/FDCxFamily.h"
#include "core/FDCxMeasurementModel.h"
#include "core/FDCxSample.h"
#include "core/FDCxFeatureVector.h"
#include "core/FDCxCapability.h"
#include "core/FDCxChipId.h"
#include "devices/fdc2x1x/FDC2x1xTypes.h"
#include "devices/fdc2x1x/FDC2x1xRegisters.h"
#include "devices/fdc2x1x/FDC2x1xFrequency.h"
#include "devices/fdc2x1x/FDC2x1xDerivedCapacitance.h"
#include "devices/fdc2x1x/FDC2x1xResonator.h"
#include "devices/fdc2x1x/FDC2x1xConversion.h"
#include "devices/fdc2x1x/FDC2x1x.h"
#include "calibration/fdc2x1x/FDC2x1xCalibrationProfile.h"
#define FDCXCAPSENSE_FDC2X1X_HEADER 1
