#pragma once

#include <stdint.h>

namespace fdc1004_registers {

constexpr uint8_t kDefaultI2cAddress = 0x50;

constexpr uint8_t kMeasurement1Msb = 0x00;
constexpr uint8_t kMeasurement1Lsb = 0x01;
constexpr uint8_t kMeasurement2Msb = 0x02;
constexpr uint8_t kMeasurement2Lsb = 0x03;
constexpr uint8_t kMeasurement3Msb = 0x04;
constexpr uint8_t kMeasurement3Lsb = 0x05;
constexpr uint8_t kMeasurement4Msb = 0x06;
constexpr uint8_t kMeasurement4Lsb = 0x07;

constexpr uint8_t kMeasurementConfig1 = 0x08;
constexpr uint8_t kMeasurementConfig2 = 0x09;
constexpr uint8_t kMeasurementConfig3 = 0x0A;
constexpr uint8_t kMeasurementConfig4 = 0x0B;
constexpr uint8_t kFdcConfig = 0x0C;

constexpr uint8_t kManufacturerId = 0xFE;
constexpr uint8_t kDeviceId = 0xFF;

constexpr uint16_t kExpectedManufacturerId = 0x5449;
constexpr uint16_t kExpectedDeviceId = 0x1004;

constexpr uint8_t kChbCapdac = 0x04;
constexpr uint8_t kChbDisabled = 0x07;

constexpr uint8_t kMaxCapdacCode = 31;
constexpr float kCapdacStepPf = 3.125f;
constexpr float kRawDenominator = 524288.0f;  // 2^19, per TI measurement conversion formula.

constexpr uint16_t kRateMask = 0x0C00;
constexpr uint16_t kRepeatBit = 0x0100;
constexpr uint16_t kMeasurementTriggerMask = 0x00F0;
constexpr uint16_t kDoneMask = 0x000F;

inline bool isResultRegister(uint8_t reg) {
  return reg <= kMeasurement4Lsb;
}

inline bool isMeasurementConfigRegister(uint8_t reg) {
  return reg >= kMeasurementConfig1 && reg <= kMeasurementConfig4;
}

inline bool isKnownReadableRegister(uint8_t reg) {
  return isResultRegister(reg) || isMeasurementConfigRegister(reg) || reg == kFdcConfig ||
         reg == kManufacturerId || reg == kDeviceId;
}

inline bool isKnownWritableRegister(uint8_t reg) {
  return isMeasurementConfigRegister(reg) || reg == kFdcConfig;
}

}  // namespace fdc1004_registers
