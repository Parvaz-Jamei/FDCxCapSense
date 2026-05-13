#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../../core/FDCxFamily.h"
#include "../../core/FDCxChipId.h"
#include "../../devices/fdc2x1x/FDC2x1xTypes.h"

constexpr uint32_t kFDC2x1xCalibrationProfileMagic = 0x46444332UL;  // "FDC2"
constexpr uint16_t kFDC2x1xCalibrationProfileVersion = 1;
constexpr size_t kFDC2x1xCalibrationProfileSerializedSize = 68;

struct FDC2x1xCalibrationProfile {
  uint32_t magic = kFDC2x1xCalibrationProfileMagic;
  uint16_t version = kFDC2x1xCalibrationProfileVersion;
  FDCxFamily family = FDCxFamily::FDC2x1x_ResonantLC;
  FDCxChipId chip = FDCxChipId::FDC2214;
  FDC2x1xVariant variant = FDC2x1xVariant::FDC2214;
  float inductanceUH[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float parasiticCapacitancePf[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float baselineFrequencyHz[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float referenceClockHz = 0.0f;
  uint32_t crc = 0;
};

bool fdc2x1xCalibrationHeaderMatches(const FDC2x1xCalibrationProfile& profile);
bool fdc2x1xProfileFieldsAreValid(const FDC2x1xCalibrationProfile& profile);
bool fdc2x1xUpdateProfileCrc(FDC2x1xCalibrationProfile& profile);
bool fdc2x1xProfileCrcIsValid(const FDC2x1xCalibrationProfile& profile);
bool fdc2x1xSerializeProfile(const FDC2x1xCalibrationProfile& profile, uint8_t* output, size_t outputLength);
bool fdc2x1xDeserializeProfile(const uint8_t* input, size_t inputLength, FDC2x1xCalibrationProfile& profile);
