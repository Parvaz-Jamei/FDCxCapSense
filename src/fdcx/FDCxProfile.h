#pragma once

#include <stddef.h>
#include <stdint.h>
#include "../core/FDCxFamily.h"
#include "../core/FDCxChipId.h"

constexpr uint32_t kFDC1004CalibrationProfileMagic = 0x46444358UL;  // "FDCX"
constexpr uint16_t kFDC1004CalibrationProfileVersion = 2;
constexpr size_t kFDC1004CalibrationProfileSerializedSize = 48;

struct FDC1004CalibrationProfile {
  uint32_t magic = kFDC1004CalibrationProfileMagic;
  uint16_t version = kFDC1004CalibrationProfileVersion;
  FDCxFamily family = FDCxFamily::FDC1004_DirectCDC;
  FDCxChipId chip = FDCxChipId::FDC1004;
  uint8_t capdacCode[4] = {0, 0, 0, 0};
  float baselinePf[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  float alphaPfPerC[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  uint32_t crc = 0;
};

uint32_t fdcxProfileCrc32(const uint8_t* data, size_t length);
bool fdcxProfileFieldsAreValid(const FDC1004CalibrationProfile& profile);
bool fdcxUpdateProfileCrc(FDC1004CalibrationProfile& profile);
bool fdcxProfileCrcIsValid(const FDC1004CalibrationProfile& profile);
bool fdcxSerializeProfile(const FDC1004CalibrationProfile& profile, uint8_t* output, size_t outputLength);
bool fdcxDeserializeProfile(const uint8_t* input, size_t inputLength, FDC1004CalibrationProfile& profile);
