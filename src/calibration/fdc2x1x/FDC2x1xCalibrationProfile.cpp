#include "FDC2x1xCalibrationProfile.h"
#include "../../fdcx/FDCxProfile.h"
#include "../../core/FDCxMath.h"
#include "../../core/FDCxSerialization.h"

namespace {

FDCxChipId expectedChipForVariant(FDC2x1xVariant variant) {
  switch (variant) {
    case FDC2x1xVariant::FDC2112: return FDCxChipId::FDC2112;
    case FDC2x1xVariant::FDC2114: return FDCxChipId::FDC2114;
    case FDC2x1xVariant::FDC2212: return FDCxChipId::FDC2212;
    case FDC2x1xVariant::FDC2214: return FDCxChipId::FDC2214;
  }
  return FDCxChipId::Unknown;
}

bool serializeWithoutCrc(const FDC2x1xCalibrationProfile& profile, uint8_t* output, size_t outputLength) {
  if (output == nullptr || outputLength < kFDC2x1xCalibrationProfileSerializedSize) return false;
  size_t offset = 0;
  fdcxPutU32(output + offset, profile.magic); offset += 4;
  fdcxPutU16(output + offset, profile.version); offset += 2;
  output[offset++] = static_cast<uint8_t>(profile.family);
  output[offset++] = static_cast<uint8_t>(profile.chip);
  output[offset++] = static_cast<uint8_t>(profile.variant);
  output[offset++] = 0; output[offset++] = 0; output[offset++] = 0;
  for (uint8_t i = 0; i < 4; ++i) { fdcxPutFloat(output + offset, profile.inductanceUH[i]); offset += 4; }
  for (uint8_t i = 0; i < 4; ++i) { fdcxPutFloat(output + offset, profile.parasiticCapacitancePf[i]); offset += 4; }
  for (uint8_t i = 0; i < 4; ++i) { fdcxPutFloat(output + offset, profile.baselineFrequencyHz[i]); offset += 4; }
  fdcxPutFloat(output + offset, profile.referenceClockHz); offset += 4;
  fdcxPutU32(output + offset, 0);
  return true;
}
}  // namespace

bool fdc2x1xCalibrationHeaderMatches(const FDC2x1xCalibrationProfile& profile) {
  return profile.magic == kFDC2x1xCalibrationProfileMagic &&
         profile.version == kFDC2x1xCalibrationProfileVersion &&
         profile.family == FDCxFamily::FDC2x1x_ResonantLC &&
         fdc2x1xVariantIsKnown(profile.variant) &&
         profile.chip == expectedChipForVariant(profile.variant);
}

bool fdc2x1xProfileFieldsAreValid(const FDC2x1xCalibrationProfile& profile) {
  if (!fdc2x1xCalibrationHeaderMatches(profile)) return false;
  if (!fdcxIsFiniteFloat(profile.referenceClockHz) || profile.referenceClockHz < 1000000.0f ||
      profile.referenceClockHz > 80000000.0f) return false;
  const uint8_t channels = fdc2x1xVariantChannelCount(profile.variant);
  for (uint8_t i = 0; i < 4; ++i) {
    if (!fdcxIsFiniteFloat(profile.inductanceUH[i]) || !fdcxIsFiniteFloat(profile.parasiticCapacitancePf[i]) ||
        !fdcxIsFiniteFloat(profile.baselineFrequencyHz[i])) {
      return false;
    }
    if (profile.parasiticCapacitancePf[i] < 0.0f || profile.parasiticCapacitancePf[i] > 250000.0f) return false;
    if (i < channels) {
      if (profile.inductanceUH[i] < 0.01f || profile.inductanceUH[i] > 100000.0f ||
          profile.baselineFrequencyHz[i] < 1000.0f || profile.baselineFrequencyHz[i] > 100000000.0f) return false;
    }
  }
  return true;
}

bool fdc2x1xUpdateProfileCrc(FDC2x1xCalibrationProfile& profile) {
  profile.magic = kFDC2x1xCalibrationProfileMagic;
  profile.version = kFDC2x1xCalibrationProfileVersion;
  profile.family = FDCxFamily::FDC2x1x_ResonantLC;
  profile.chip = expectedChipForVariant(profile.variant);
  if (!fdc2x1xProfileFieldsAreValid(profile)) return false;
  uint8_t buffer[kFDC2x1xCalibrationProfileSerializedSize] = {};
  if (!serializeWithoutCrc(profile, buffer, sizeof(buffer))) return false;
  profile.crc = fdcxProfileCrc32(buffer, kFDC2x1xCalibrationProfileSerializedSize - 4u);
  return true;
}

bool fdc2x1xProfileCrcIsValid(const FDC2x1xCalibrationProfile& profile) {
  if (!fdc2x1xProfileFieldsAreValid(profile)) return false;
  uint8_t buffer[kFDC2x1xCalibrationProfileSerializedSize] = {};
  if (!serializeWithoutCrc(profile, buffer, sizeof(buffer))) return false;
  const uint32_t actual = fdcxProfileCrc32(buffer, kFDC2x1xCalibrationProfileSerializedSize - 4u);
  return actual == profile.crc;
}

bool fdc2x1xSerializeProfile(const FDC2x1xCalibrationProfile& profile, uint8_t* output, size_t outputLength) {
  if (output == nullptr || outputLength < kFDC2x1xCalibrationProfileSerializedSize) return false;
  if (!fdc2x1xProfileFieldsAreValid(profile)) return false;
  FDC2x1xCalibrationProfile copy = profile;
  if (!fdc2x1xUpdateProfileCrc(copy)) return false;
  if (!serializeWithoutCrc(copy, output, outputLength)) return false;
  fdcxPutU32(output + kFDC2x1xCalibrationProfileSerializedSize - 4u, copy.crc);
  return true;
}

bool fdc2x1xDeserializeProfile(const uint8_t* input, size_t inputLength, FDC2x1xCalibrationProfile& profile) {
  if (input == nullptr || inputLength < kFDC2x1xCalibrationProfileSerializedSize) return false;
  size_t offset = 0;
  FDC2x1xCalibrationProfile parsed;
  parsed.magic = fdcxGetU32(input + offset); offset += 4;
  parsed.version = fdcxGetU16(input + offset); offset += 2;
  parsed.family = static_cast<FDCxFamily>(input[offset++]);
  parsed.chip = static_cast<FDCxChipId>(input[offset++]);
  parsed.variant = static_cast<FDC2x1xVariant>(input[offset++]);
  offset += 3;
  for (uint8_t i = 0; i < 4; ++i) { parsed.inductanceUH[i] = fdcxGetFloat(input + offset); offset += 4; }
  for (uint8_t i = 0; i < 4; ++i) { parsed.parasiticCapacitancePf[i] = fdcxGetFloat(input + offset); offset += 4; }
  for (uint8_t i = 0; i < 4; ++i) { parsed.baselineFrequencyHz[i] = fdcxGetFloat(input + offset); offset += 4; }
  parsed.referenceClockHz = fdcxGetFloat(input + offset); offset += 4;
  parsed.crc = fdcxGetU32(input + offset);
  if (!fdc2x1xProfileCrcIsValid(parsed)) return false;
  profile = parsed;
  return true;
}
