#pragma once
#include <stdint.h>
#include "FDC2x1xTypes.h"

namespace fdc2x1x_registers {
constexpr uint8_t kDefaultI2cAddressLow = 0x2A;
constexpr uint8_t kDefaultI2cAddressHigh = 0x2B;
constexpr uint8_t kDataCh0Msb = 0x00;
constexpr uint8_t kDataCh0Lsb = 0x01;
constexpr uint8_t kDataCh1Msb = 0x02;
constexpr uint8_t kDataCh1Lsb = 0x03;
constexpr uint8_t kDataCh2Msb = 0x04;
constexpr uint8_t kDataCh2Lsb = 0x05;
constexpr uint8_t kDataCh3Msb = 0x06;
constexpr uint8_t kDataCh3Lsb = 0x07;
constexpr uint8_t kRCountCh0 = 0x08;
constexpr uint8_t kRCountCh3 = 0x0B;
constexpr uint8_t kOffsetCh0 = 0x0C;
constexpr uint8_t kOffsetCh3 = 0x0F;
constexpr uint8_t kSettleCountCh0 = 0x10;
constexpr uint8_t kSettleCountCh3 = 0x13;
constexpr uint8_t kClockDividersCh0 = 0x14;
constexpr uint8_t kClockDividersCh3 = 0x17;
constexpr uint8_t kStatus = 0x18;
constexpr uint8_t kErrorConfig = 0x19;
constexpr uint8_t kConfig = 0x1A;
constexpr uint8_t kMuxConfig = 0x1B;
constexpr uint8_t kResetDev = 0x1C;
constexpr uint8_t kDriveCurrentCh0 = 0x1E;
constexpr uint8_t kDriveCurrentCh3 = 0x21;
constexpr uint8_t kManufacturerId = 0x7E;
constexpr uint8_t kDeviceId = 0x7F;

constexpr uint16_t kExpectedManufacturerId = 0x5449u;
constexpr uint16_t kExpectedDeviceIdFdc211x = 0x3054u;
constexpr uint16_t kExpectedDeviceIdFdc221x = 0x3055u;

constexpr uint16_t kStatusDrdyMask = 0x0040u;
constexpr uint16_t kStatusDataReadyMask = kStatusDrdyMask;  // Compatibility alias.
constexpr uint16_t kStatusCh0UnreadMask = 0x0008u;
constexpr uint16_t kStatusCh1UnreadMask = 0x0004u;
constexpr uint16_t kStatusCh2UnreadMask = 0x0002u;
constexpr uint16_t kStatusCh3UnreadMask = 0x0001u;
constexpr uint16_t kStatusErrorMask = 0x0800u | 0x0400u | 0x0200u;  // ERR_WD, ERR_AHW, ERR_ALW.

constexpr uint16_t kDataErrWdMask = 0x2000u;
constexpr uint16_t kDataErrAwMask = 0x1000u;
constexpr uint16_t kDataErrorMask = kDataErrWdMask | kDataErrAwMask;
constexpr uint16_t kDataMsbMask = 0x0FFFu;

constexpr uint16_t kClockFinSelMask = 0x3000u;
constexpr uint8_t kClockFinSelShift = 12u;
constexpr uint16_t kClockFrefDividerMask = 0x03FFu;
constexpr uint16_t kClockDividerAllowedMask = kClockFinSelMask | kClockFrefDividerMask;
constexpr uint16_t kDriveCurrentCodeMask = 0xF800u;
constexpr uint16_t kConfigActiveChannelMask = 0xC000u;
constexpr uint8_t kConfigActiveChannelShift = 14u;
constexpr uint16_t kConfigSleepModeMask = 0x2000u;
constexpr uint16_t kConfigSensorActivateMask = 0x0800u;
constexpr uint16_t kConfigRefClockSrcMask = 0x0200u;
constexpr uint16_t kConfigIntbDisableMask = 0x0080u;
constexpr uint16_t kConfigHighCurrentMask = 0x0040u;
constexpr uint16_t kResetDevResetMask = 0x8000u;
constexpr uint16_t kResetDevOutputGainMask = 0x0600u;
constexpr uint16_t kResetDevFdc211xAllowedMask = kResetDevResetMask | kResetDevOutputGainMask;
constexpr uint16_t kResetDevFdc221xAllowedMask = kResetDevResetMask;
constexpr uint16_t kErrorConfigAllowedMask = 0x2000u | 0x1000u | 0x0800u | 0x0020u | 0x0001u;
constexpr uint16_t kConfigOptionPreserveMask = kConfigSensorActivateMask | kConfigRefClockSrcMask |
                                                kConfigIntbDisableMask | kConfigHighCurrentMask;
constexpr uint16_t kConfigReservedRequiredOnes = 0x1401u;  // Bit12 + bit10 + bits5:0 = 000001.
constexpr uint16_t kConfigReservedMask = 0x1000u | 0x0400u | 0x0100u | 0x003Fu;
constexpr uint16_t kConfigReservedPattern = kConfigReservedRequiredOnes;  // Compatibility alias for safe defaults.
constexpr uint16_t kMuxConfigAutoscanMask = 0x8000u;
constexpr uint16_t kMuxConfigRrSequenceMask = 0x6000u;
constexpr uint8_t kMuxConfigRrSequenceShift = 13u;
constexpr uint16_t kMuxConfigAutoscanSequenceMask = kMuxConfigAutoscanMask | kMuxConfigRrSequenceMask;
constexpr uint16_t kMuxConfigReservedPattern = 0x0208u;
constexpr uint16_t kMuxConfigDeglitchMask = 0x0007u;
constexpr uint16_t kMuxConfigPreserveMask = kMuxConfigAutoscanSequenceMask;  // Compatibility alias.
constexpr uint32_t kRawDenominator28 = 268435456UL;
constexpr uint32_t kRawDenominator12 = 4096UL;

inline uint8_t dataMsbRegister(uint8_t channel) { return static_cast<uint8_t>(kDataCh0Msb + channel * 2u); }
inline uint8_t dataLsbRegister(uint8_t channel) { return static_cast<uint8_t>(kDataCh0Lsb + channel * 2u); }
inline uint8_t rcountRegister(uint8_t channel) { return static_cast<uint8_t>(kRCountCh0 + channel); }
inline uint8_t offsetRegister(uint8_t channel) { return static_cast<uint8_t>(kOffsetCh0 + channel); }
inline uint8_t settleCountRegister(uint8_t channel) { return static_cast<uint8_t>(kSettleCountCh0 + channel); }
inline uint8_t clockDividersRegister(uint8_t channel) { return static_cast<uint8_t>(kClockDividersCh0 + channel); }
inline uint8_t driveCurrentRegister(uint8_t channel) { return static_cast<uint8_t>(kDriveCurrentCh0 + channel); }

inline bool isDataRegister(uint8_t reg) { return reg <= kDataCh3Lsb; }
inline bool isChannelRegister(uint8_t reg, uint8_t base, uint8_t& channel) {
  if (reg < base || reg > static_cast<uint8_t>(base + 3u)) return false;
  channel = static_cast<uint8_t>(reg - base);
  return true;
}
inline bool isKnownReadableRegister(uint8_t reg) {
  return isDataRegister(reg) || reg == kStatus || reg == kErrorConfig || reg == kConfig || reg == kMuxConfig ||
         reg == kManufacturerId || reg == kDeviceId || reg == kResetDev ||
         (reg >= kRCountCh0 && reg <= kRCountCh3) ||
         (reg >= kOffsetCh0 && reg <= kOffsetCh3) ||
         (reg >= kSettleCountCh0 && reg <= kSettleCountCh3) ||
         (reg >= kClockDividersCh0 && reg <= kClockDividersCh3) ||
         (reg >= kDriveCurrentCh0 && reg <= kDriveCurrentCh3);
}
inline bool isKnownWritableRegister(uint8_t reg) {
  return (reg >= kRCountCh0 && reg <= kRCountCh3) ||
         (reg >= kOffsetCh0 && reg <= kOffsetCh3) ||
         (reg >= kSettleCountCh0 && reg <= kSettleCountCh3) ||
         (reg >= kClockDividersCh0 && reg <= kClockDividersCh3) ||
         reg == kErrorConfig || reg == kConfig || reg == kMuxConfig || reg == kResetDev ||
         (reg >= kDriveCurrentCh0 && reg <= kDriveCurrentCh3);
}
inline uint16_t unreadMaskForChannel(FDC2x1xChannel channel) {
  switch (channel) {
    case FDC2x1xChannel::CH0: return kStatusCh0UnreadMask;
    case FDC2x1xChannel::CH1: return kStatusCh1UnreadMask;
    case FDC2x1xChannel::CH2: return kStatusCh2UnreadMask;
    case FDC2x1xChannel::CH3: return kStatusCh3UnreadMask;
  }
  return 0u;
}
inline uint16_t finSelBitsForMultiplier(uint8_t finSelMultiplier) {
  return static_cast<uint16_t>((finSelMultiplier == 1u ? 1u : 2u) << kClockFinSelShift);
}
inline bool decodeFinSelMultiplier(uint16_t clockDividerReg, uint8_t& finSelMultiplier) {
  const uint8_t encoded = static_cast<uint8_t>((clockDividerReg & kClockFinSelMask) >> kClockFinSelShift);
  if (encoded == 1u) { finSelMultiplier = 1u; return true; }
  if (encoded == 2u) { finSelMultiplier = 2u; return true; }
  finSelMultiplier = 0u;
  return false;
}
inline uint16_t decodeFrefDivider(uint16_t clockDividerReg) {
  return static_cast<uint16_t>(clockDividerReg & kClockFrefDividerMask);
}

inline uint8_t activeChannelFromConfig(uint16_t value) {
  return static_cast<uint8_t>((value & kConfigActiveChannelMask) >> kConfigActiveChannelShift);
}
inline uint8_t rrSequenceFromMuxConfig(uint16_t value) {
  return static_cast<uint8_t>((value & kMuxConfigRrSequenceMask) >> kMuxConfigRrSequenceShift);
}
inline bool muxAutoscanEnabled(uint16_t value) {
  return (value & kMuxConfigAutoscanMask) != 0u;
}
inline bool configReservedPatternIsValid(uint16_t value) {
  return (value & kConfigReservedMask) == kConfigReservedRequiredOnes;
}
inline bool muxConfigReservedPatternIsValid(uint16_t value) {
  return (value & static_cast<uint16_t>(~(kMuxConfigAutoscanSequenceMask | kMuxConfigDeglitchMask))) == kMuxConfigReservedPattern;
}
inline uint16_t configValueFor(FDC2x1xChannel channel, bool sleepEnabled, uint16_t preservedOptions = 0u) {
  return static_cast<uint16_t>(((static_cast<uint16_t>(channel) << kConfigActiveChannelShift) & kConfigActiveChannelMask) |
                               (sleepEnabled ? kConfigSleepModeMask : 0u) |
                               (preservedOptions & kConfigOptionPreserveMask) |
                               kConfigReservedRequiredOnes);
}
inline uint16_t singleChannelMuxValue(FDC2x1xDeglitch deglitch) {
  return static_cast<uint16_t>(kMuxConfigReservedPattern |
                               (static_cast<uint16_t>(deglitch) & kMuxConfigDeglitchMask));
}
}  // namespace fdc2x1x_registers
