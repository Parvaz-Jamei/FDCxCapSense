#pragma once

#include <stdint.h>
#include <FDCxCapSense.h>

class FakeI2C final : public FDCxI2C {
 public:
  FakeI2C() {
    reset();
  }

  void reset() {
    for (uint16_t i = 0; i < 256; ++i) {
      registers[i] = 0;
    }
    registers[fdc1004_registers::kManufacturerId] = fdc1004_registers::kExpectedManufacturerId;
    registers[fdc1004_registers::kDeviceId] = fdc1004_registers::kExpectedDeviceId;
    for (uint8_t i = 0; i < 4; ++i) {
      simulatedCapacitancePf[i] = 0.0f;
      autoCompleteMeasurement[i] = true;
    }
    fdc2x1xMode = false;
    fdc2x1xVariant = FDC2x1xVariant::FDC2214;
    badMuxConfigWrite = false;
    badConfigWrite = false;
    failRead = false;
    failWrite = false;
    expectedAddress = fdc1004_registers::kDefaultI2cAddress;
    readCount = 0;
    writeCount = 0;
    writeLogCount = 0;
    for (uint16_t i = 0; i < 256; ++i) {
      readHits[i] = 0;
      writeHits[i] = 0;
    }
  }

  void resetFdc2x1x(FDC2x1xVariant variant = FDC2x1xVariant::FDC2214) {
    for (uint16_t i = 0; i < 256; ++i) {
      registers[i] = 0;
    }
    fdc2x1xMode = true;
    fdc2x1xVariant = variant;
    badMuxConfigWrite = false;
    badConfigWrite = false;
    failRead = false;
    failWrite = false;
    expectedAddress = fdc2x1x_registers::kDefaultI2cAddressLow;
    readCount = 0;
    writeCount = 0;
    writeLogCount = 0;
    for (uint16_t i = 0; i < 256; ++i) {
      readHits[i] = 0;
      writeHits[i] = 0;
    }
    registers[fdc2x1x_registers::kManufacturerId] = fdc2x1x_registers::kExpectedManufacturerId;
    registers[fdc2x1x_registers::kDeviceId] = fdc2x1xVariantIsFdc211x(variant)
                                                ? fdc2x1x_registers::kExpectedDeviceIdFdc211x
                                                : fdc2x1x_registers::kExpectedDeviceIdFdc221x;
    registers[fdc2x1x_registers::kConfig] = 0x2801u;
    registers[fdc2x1x_registers::kMuxConfig] = 0x020Fu;
    registers[fdc2x1x_registers::dataMsbRegister(0)] = 0x0800u;
    registers[fdc2x1x_registers::dataLsbRegister(0)] = 0x0000u;
    refreshFdc2Status();
  }

  FDCxStatus readRegister16(uint8_t address, uint8_t reg, uint16_t& value) override {
    ++readCount;
    ++readHits[reg];
    if (failRead || address != expectedAddress) {
      return FDCxStatus::I2cError;
    }
    if (fdc2x1xMode && reg == fdc2x1x_registers::kStatus) {
      refreshFdc2Status();
    }
    value = registers[reg];
    return FDCxStatus::Ok;
  }

  FDCxStatus writeRegister16(uint8_t address, uint8_t reg, uint16_t value) override {
    ++writeCount;
    ++writeHits[reg];
    if (failWrite || address != expectedAddress) {
      return FDCxStatus::I2cError;
    }
    if (fdc2x1xMode) {
      if (reg == fdc2x1x_registers::kConfig &&
          (!fdc2x1x_registers::configReservedPatternIsValid(value) ||
           fdc2x1x_registers::activeChannelFromConfig(value) >= fdc2x1xVariantChannelCount(fdc2x1xVariant) ||
           (((value & fdc2x1x_registers::kConfigHighCurrentMask) != 0u) &&
            fdc2x1x_registers::activeChannelFromConfig(value) != 0u) ||
           (((value & fdc2x1x_registers::kConfigHighCurrentMask) != 0u) &&
            fdc2x1x_registers::muxAutoscanEnabled(registers[fdc2x1x_registers::kMuxConfig])))) {
        badConfigWrite = true;
        return FDCxStatus::InvalidArgument;
      }
      if (reg == fdc2x1x_registers::kMuxConfig &&
          (!fdc2x1x_registers::muxConfigReservedPatternIsValid(value) ||
           !muxSequenceAllowedForVariant(value) ||
           (fdc2x1x_registers::muxAutoscanEnabled(value) &&
            (registers[fdc2x1x_registers::kConfig] & fdc2x1x_registers::kConfigHighCurrentMask) != 0u))) {
        badMuxConfigWrite = true;
        return FDCxStatus::InvalidArgument;
      }
      if (reg == fdc2x1x_registers::kErrorConfig &&
          ((value & static_cast<uint16_t>(~fdc2x1x_registers::kErrorConfigAllowedMask)) != 0u)) {
        return FDCxStatus::InvalidArgument;
      }
      if (reg == fdc2x1x_registers::kResetDev && !resetDevAllowedForVariant(value)) {
        return FDCxStatus::InvalidArgument;
      }
    }

    if (writeLogCount < 64u) {
      writeLogReg[writeLogCount] = reg;
      writeLogValue[writeLogCount] = value;
      ++writeLogCount;
    }

    if (fdc2x1xMode && reg == fdc2x1x_registers::kResetDev &&
        (value & fdc2x1x_registers::kResetDevResetMask) != 0u) {
      applyFdc2ResetDefaults();
      return FDCxStatus::Ok;
    }

    registers[reg] = value;

    if (fdc2x1xMode && (reg == fdc2x1x_registers::kConfig || reg == fdc2x1x_registers::kMuxConfig)) {
      refreshFdc2Status();
      return FDCxStatus::Ok;
    }

    if (reg == fdc1004_registers::kFdcConfig) {
      for (uint8_t index = 0; index < 4; ++index) {
        const uint16_t triggerBit = static_cast<uint16_t>(1u << (7u - index));
        if ((value & triggerBit) != 0u && autoCompleteMeasurement[index]) {
          simulateResult(index);
          registers[fdc1004_registers::kFdcConfig] = static_cast<uint16_t>(registers[fdc1004_registers::kFdcConfig] |
                                                                           static_cast<uint16_t>(1u << (3u - index)));
        }
      }
    }
    return FDCxStatus::Ok;
  }

  void setRegister(uint8_t reg, uint16_t value) {
    registers[reg] = value;
    if (fdc2x1xMode && (reg == fdc2x1x_registers::kConfig || reg == fdc2x1x_registers::kMuxConfig)) {
      refreshFdc2Status();
    }
  }

  uint16_t getRegister(uint8_t reg) const {
    return registers[reg];
  }

  void setSimulatedCapacitance(uint8_t measurementIndex, float capacitancePf) {
    if (measurementIndex < 4u) {
      simulatedCapacitancePf[measurementIndex] = capacitancePf;
    }
  }

  uint16_t registers[256] = {};
  bool failRead = false;
  bool failWrite = false;
  bool fdc2x1xMode = false;
  FDC2x1xVariant fdc2x1xVariant = FDC2x1xVariant::FDC2214;
  bool badMuxConfigWrite = false;
  bool badConfigWrite = false;
  bool autoCompleteMeasurement[4] = {true, true, true, true};
  float simulatedCapacitancePf[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  uint8_t expectedAddress = fdc1004_registers::kDefaultI2cAddress;
  uint16_t readCount = 0;
  uint16_t writeCount = 0;
  uint16_t readHits[256] = {};
  uint16_t writeHits[256] = {};
  uint8_t writeLogReg[64] = {};
  uint16_t writeLogValue[64] = {};
  uint8_t writeLogCount = 0;

 private:
  bool muxSequenceAllowedForVariant(uint16_t value) const {
    if (!fdc2x1x_registers::muxAutoscanEnabled(value)) {
      return true;
    }
    const uint8_t rr = fdc2x1x_registers::rrSequenceFromMuxConfig(value);
    if (fdc2x1xVariantChannelCount(fdc2x1xVariant) <= 2u) {
      return rr == 0u || rr == 3u;
    }
    return rr <= 3u;
  }

  bool resetDevAllowedForVariant(uint16_t value) const {
    const uint16_t allowed = fdc2x1xVariantIsFdc211x(fdc2x1xVariant)
                                 ? fdc2x1x_registers::kResetDevFdc211xAllowedMask
                                 : fdc2x1x_registers::kResetDevFdc221xAllowedMask;
    return (value & static_cast<uint16_t>(~allowed)) == 0u;
  }

  void applyFdc2ResetDefaults() {
    registers[fdc2x1x_registers::kConfig] = 0x2801u;
    registers[fdc2x1x_registers::kMuxConfig] = 0x020Fu;
    registers[fdc2x1x_registers::kResetDev] = 0u;
    refreshFdc2Status();
  }

  static int32_t clampRaw(float rawFloat) {
    if (rawFloat > 8388607.0f) {
      return 8388607L;
    }
    if (rawFloat < -8388608.0f) {
      return -8388608L;
    }
    return static_cast<int32_t>(rawFloat);
  }

  void refreshFdc2Status() {
    if (!fdc2x1xMode) {
      return;
    }
    if ((registers[fdc2x1x_registers::kConfig] & fdc2x1x_registers::kConfigSleepModeMask) != 0u ||
        !fdc2x1x_registers::configReservedPatternIsValid(registers[fdc2x1x_registers::kConfig]) ||
        !fdc2x1x_registers::muxConfigReservedPatternIsValid(registers[fdc2x1x_registers::kMuxConfig])) {
      registers[fdc2x1x_registers::kStatus] = 0u;
      return;
    }
    const uint8_t active = fdc2x1x_registers::activeChannelFromConfig(registers[fdc2x1x_registers::kConfig]);
    if (active >= fdc2x1xVariantChannelCount(fdc2x1xVariant)) {
      registers[fdc2x1x_registers::kStatus] = 0u;
      return;
    }
    registers[fdc2x1x_registers::kStatus] = fdc2x1x_registers::unreadMaskForChannel(static_cast<FDC2x1xChannel>(active));
  }

  void simulateResult(uint8_t index) {
    if (index >= 4u) {
      return;
    }
    const uint8_t configReg = static_cast<uint8_t>(fdc1004_registers::kMeasurementConfig1 + index);
    const uint16_t config = registers[configReg];
    const uint8_t chb = static_cast<uint8_t>((config >> 10) & 0x07u);
    const uint8_t capdac = static_cast<uint8_t>((config >> 5) & 0x1Fu);
    const float offsetPf = (chb == fdc1004_registers::kChbCapdac)
                               ? static_cast<float>(capdac) * fdc1004_registers::kCapdacStepPf
                               : 0.0f;
    const float rawPf = simulatedCapacitancePf[index] - offsetPf;
    const int32_t raw = clampRaw(rawPf * fdc1004_registers::kRawDenominator);
    const uint32_t rawUnsigned = static_cast<uint32_t>(raw) & 0x00FFFFFFu;
    const uint16_t msb = static_cast<uint16_t>((rawUnsigned >> 8) & 0xFFFFu);
    const uint16_t lsb = static_cast<uint16_t>((rawUnsigned & 0xFFu) << 8);
    registers[static_cast<uint8_t>(fdc1004_registers::kMeasurement1Msb + (index * 2u))] = msb;
    registers[static_cast<uint8_t>(fdc1004_registers::kMeasurement1Lsb + (index * 2u))] = lsb;
  }
};
