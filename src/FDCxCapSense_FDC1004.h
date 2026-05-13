#pragma once

// Lightweight FDC1004-only public include for UNO/AVR and projects that do not need experimental profiles.
// FDC2x1x support is provided through the separate FDCxCapSense_FDC2x1x.h header in v0.6.11; this header intentionally stays FDC1004-only.

#include "core/FDCStatus.h"
#include "core/FDCI2C.h"
#include "core/FDCxFamily.h"
#include "core/FDCxMeasurementModel.h"
#include "core/FDCxSample.h"
#include "core/FDCxFeatureVector.h"
#include "core/FDCxCapability.h"
#include "core/FDCxChipId.h"
#include "devices/fdc1004/FDC1004.h"
#include "devices/fdc1004/FDC1004Reading.h"
#include "calibration/fdc1004/CapdacManager.h"
#include "calibration/fdc1004/FDC1004Calibration.h"
#include "calibration/fdc1004/FDC1004BaselineTracker.h"
#include "calibration/fdc1004/FDC1004CalibrationProfile.h"
#include "calibration/shared/TemperatureCompensator.h"
#include "calibration/shared/CalibrationStorage.h"

#define FDCXCAPSENSE_FDC1004_HEADER 1

#ifndef FDCXCAPSENSE_METADATA_FUNCTIONS_DEFINED
#define FDCXCAPSENSE_METADATA_FUNCTIONS_DEFINED 1
namespace fdcx_capsense {
constexpr const char* projectName() {
  return "FDCxCapSense";
}

constexpr const char* phaseStatus() {
  return "v0.6.11 Final Industrial Quality Polish: FDC1004 direct-CDC driver, metadata-aware profile contracts, separate FDC2x1x frequency-first foundation, no Developer hardware validation claim";
}
}  // namespace fdcx_capsense
#endif
