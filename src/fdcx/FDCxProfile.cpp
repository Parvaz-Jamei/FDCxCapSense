#include "FDCxProfile.h"
#include "../core/FDCxMath.h"
#include "../core/FDCxSerialization.h"

namespace {

bool serializeWithoutCrc(const FDC1004CalibrationProfile& profile, uint8_t* output, size_t outputLength) {
  if (output == nullptr || outputLength < kFDC1004CalibrationProfileSerializedSize) {
    return false;
  }
  size_t offset = 0;
  fdcxPutU32(output + offset, profile.magic);
  offset += 4;
  fdcxPutU16(output + offset, profile.version);
  offset += 2;
  output[offset++] = static_cast<uint8_t>(profile.family);
  output[offset++] = static_cast<uint8_t>(profile.chip);
  for (uint8_t i = 0; i < 4; ++i) {
    output[offset++] = profile.capdacCode[i];
  }
  for (uint8_t i = 0; i < 4; ++i) {
    fdcxPutFloat(output + offset, profile.baselinePf[i]);
    offset += 4;
  }
  for (uint8_t i = 0; i < 4; ++i) {
    fdcxPutFloat(output + offset, profile.alphaPfPerC[i]);
    offset += 4;
  }
  fdcxPutU32(output + offset, 0);
  return true;
}

bool fdcxProfileHeaderIsValid(const FDC1004CalibrationProfile& profile) {
  return profile.magic == kFDC1004CalibrationProfileMagic &&
         profile.version == kFDC1004CalibrationProfileVersion &&
         profile.family == FDCxFamily::FDC1004_DirectCDC &&
         (profile.chip == FDCxChipId::FDC1004 || profile.chip == FDCxChipId::FDC1004_Q1);
}
}  // namespace

uint32_t fdcxProfileCrc32(const uint8_t* data, size_t length) {
  if (data == nullptr) {
    return 0;
  }
  uint32_t crc = 0xFFFFFFFFUL;
  for (size_t i = 0; i < length; ++i) {
    crc ^= static_cast<uint32_t>(data[i]);
    for (uint8_t bit = 0; bit < 8; ++bit) {
      const uint32_t mask = static_cast<uint32_t>(0u - (crc & 1u));
      crc = (crc >> 1) ^ (0xEDB88320UL & mask);
    }
  }
  return ~crc;
}

bool fdcxProfileFieldsAreValid(const FDC1004CalibrationProfile& profile) {
  for (uint8_t i = 0; i < 4; ++i) {
    if (profile.capdacCode[i] > 31u) {
      return false;
    }
    if (!fdcxIsFiniteFloat(profile.baselinePf[i]) || !fdcxIsFiniteFloat(profile.alphaPfPerC[i])) {
      return false;
    }
  }
  return true;
}

bool fdcxUpdateProfileCrc(FDC1004CalibrationProfile& profile) {
  profile.magic = kFDC1004CalibrationProfileMagic;
  profile.version = kFDC1004CalibrationProfileVersion;
  if (!fdcxProfileHeaderIsValid(profile) || !fdcxProfileFieldsAreValid(profile)) {
    return false;
  }
  uint8_t buffer[kFDC1004CalibrationProfileSerializedSize] = {};
  if (!serializeWithoutCrc(profile, buffer, sizeof(buffer))) {
    return false;
  }
  profile.crc = fdcxProfileCrc32(buffer, kFDC1004CalibrationProfileSerializedSize - 4u);
  return true;
}

bool fdcxProfileCrcIsValid(const FDC1004CalibrationProfile& profile) {
  // v0.4.3 hotfix: validate the original header and fields before calculating CRC.
  // Do not call fdcxUpdateProfileCrc() here because it normalizes magic/version.
  if (!fdcxProfileHeaderIsValid(profile) || !fdcxProfileFieldsAreValid(profile)) {
    return false;
  }
  uint8_t buffer[kFDC1004CalibrationProfileSerializedSize] = {};
  if (!serializeWithoutCrc(profile, buffer, sizeof(buffer))) {
    return false;
  }
  const uint32_t actual = fdcxProfileCrc32(buffer, kFDC1004CalibrationProfileSerializedSize - 4u);
  return actual == profile.crc;
}

bool fdcxSerializeProfile(const FDC1004CalibrationProfile& profile, uint8_t* output, size_t outputLength) {
  if (output == nullptr || outputLength < kFDC1004CalibrationProfileSerializedSize) {
    return false;
  }
  if (!fdcxProfileHeaderIsValid(profile) || !fdcxProfileFieldsAreValid(profile)) {
    return false;
  }
  FDC1004CalibrationProfile copy = profile;
  if (!fdcxUpdateProfileCrc(copy)) {
    return false;
  }
  if (!serializeWithoutCrc(copy, output, outputLength)) {
    return false;
  }
  fdcxPutU32(output + kFDC1004CalibrationProfileSerializedSize - 4u, copy.crc);
  return true;
}

bool fdcxDeserializeProfile(const uint8_t* input, size_t inputLength, FDC1004CalibrationProfile& profile) {
  if (input == nullptr || inputLength < kFDC1004CalibrationProfileSerializedSize) {
    return false;
  }
  size_t offset = 0;
  FDC1004CalibrationProfile parsed;
  parsed.magic = fdcxGetU32(input + offset);
  offset += 4;
  parsed.version = fdcxGetU16(input + offset);
  offset += 2;
  parsed.family = static_cast<FDCxFamily>(input[offset++]);
  parsed.chip = static_cast<FDCxChipId>(input[offset++]);
  for (uint8_t i = 0; i < 4; ++i) {
    parsed.capdacCode[i] = input[offset++];
  }
  for (uint8_t i = 0; i < 4; ++i) {
    parsed.baselinePf[i] = fdcxGetFloat(input + offset);
    offset += 4;
  }
  for (uint8_t i = 0; i < 4; ++i) {
    parsed.alphaPfPerC[i] = fdcxGetFloat(input + offset);
    offset += 4;
  }
  parsed.crc = fdcxGetU32(input + offset);

  if (!fdcxProfileCrcIsValid(parsed)) {
    return false;
  }
  profile = parsed;
  return true;
}
