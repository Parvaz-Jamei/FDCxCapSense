#pragma once

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include "FDC1004AutoCapdac.h"
#include "FDC1004Registers.h"
#include "FDCxI2C.h"
#include "FDCxStatus.h"
#include "../devices/fdc1004/FDC1004Reading.h"
#include "../core/FDCxCapability.h"

enum class FDC1004Channel : uint8_t {
  CIN1 = 0,
  CIN2 = 1,
  CIN3 = 2,
  CIN4 = 3
};

enum class FDC1004Measurement : uint8_t {
  M1 = 0,
  M2 = 1,
  M3 = 2,
  M4 = 3
};

enum class FDC1004Rate : uint8_t {
  SPS_100 = 1,
  SPS_200 = 2,
  SPS_400 = 3
};

enum class FDC1004CapdacMode : uint8_t {
  Disabled = 0,
  Enabled = 1
};

class FDC1004 {
 public:
  FDC1004() = default;

  FDCxStatus begin(TwoWire& wire = Wire, uint8_t address = fdc1004_registers::kDefaultI2cAddress);
  FDCxStatus begin(FDCxI2C& bus, uint8_t address = fdc1004_registers::kDefaultI2cAddress);

  FDCxStatus readRegister16(uint8_t reg, uint16_t& value);
  FDCxStatus writeRegister16(uint8_t reg, uint16_t value);

  void setTimeoutMs(uint16_t timeoutMs);
  FDCxStatus lastStatus() const;
  FDCxCapability capability() const;

  // Preferred safe overload: single-ended input with CAPDAC disabled (CHB=b111, CAPDAC=0).
  FDCxStatus configureSingleEnded(FDC1004Measurement measurement,
                                  FDC1004Channel channel);

  // Explicit CAPDAC-enabled path (CHB=b100, CAPDAC=0..31). capdacCode=0 is valid but intentional.
  FDCxStatus configureSingleEndedWithCapdac(FDC1004Measurement measurement,
                                            FDC1004Channel channel,
                                            uint8_t capdacCode);

  // Backward-compatible alias for Phase 1 sketches. It is intentionally CAPDAC-enabled.
  FDCxStatus configureSingleEnded(FDC1004Measurement measurement,
                                  FDC1004Channel channel,
                                  uint8_t capdacCode);

  FDCxStatus setSampleRate(FDC1004Rate rate);
  FDCxStatus setRepeatMode(bool enabled);
  FDCxStatus stopRepeatedMeasurements();
  FDCxStatus idle();
  FDCxStatus triggerSingleMeasurement(FDC1004Measurement measurement);
  FDCxStatus waitForMeasurement(FDC1004Measurement measurement, uint16_t timeoutMs);

  FDCxStatus readRaw24(FDC1004Measurement measurement, int32_t& raw24);
  // Reads the latest completed measurement registers. For a fresh single-shot conversion, use readSingleMeasurement().
  FDCxStatus readCapacitancePf(FDC1004Measurement measurement, float& capacitancePf);
  FDCxStatus readLastCapacitancePf(FDC1004Measurement measurement, float& capacitancePf);
  FDCxStatus readCapacitanceReading(FDC1004Measurement measurement, FDC1004Reading& reading);
  FDCxStatus readSingleMeasurement(FDC1004Measurement measurement, int32_t& raw24, float& capacitancePf);

  FDC1004AutoCapdacResult autoTuneCapdac(FDC1004Measurement measurement,
                                         FDC1004Channel channel,
                                         uint8_t minCode,
                                         uint8_t maxCode,
                                         float targetCenterPf,
                                         float marginPf);

  uint16_t buildSingleEndedConfig(FDC1004Channel channel,
                                  FDC1004CapdacMode mode,
                                  uint8_t capdacCode,
                                  FDCxStatus* status = nullptr) const;
  uint16_t buildSingleEndedConfig(FDC1004Channel channel, uint8_t capdacCode, FDCxStatus* status = nullptr) const;
  uint16_t currentConfigRegisterValue() const;
  uint8_t address() const;
  bool capdacEnabled(FDC1004Measurement measurement) const;
  uint8_t capdacCode(FDC1004Measurement measurement) const;

 private:
  static bool isValidMeasurement(FDC1004Measurement measurement);
  static bool isValidChannel(FDC1004Channel channel);
  static uint8_t measurementIndex(FDC1004Measurement measurement);
  static uint8_t measurementMsbRegister(FDC1004Measurement measurement);
  static uint8_t measurementLsbRegister(FDC1004Measurement measurement);
  static uint8_t measurementConfigRegister(FDC1004Measurement measurement);
  static uint16_t measurementTriggerBit(FDC1004Measurement measurement);
  static uint16_t measurementDoneBit(FDC1004Measurement measurement);
  static int32_t signExtend24(uint32_t raw24Unsigned);

  float capdacOffsetPf(FDC1004Measurement measurement) const;
  FDCxStatus configureSingleEndedInternal(FDC1004Measurement measurement,
                                          FDC1004Channel channel,
                                          FDC1004CapdacMode mode,
                                          uint8_t capdacCode);
  FDCxStatus setStatus(FDCxStatus status);
  FDCxStatus readIdentification();

  FDCxTwoWireI2C _wireAdapter;
  FDCxI2C* _bus = nullptr;
  uint8_t _address = fdc1004_registers::kDefaultI2cAddress;
  uint16_t _timeoutMs = 100;
  uint16_t _fdcConfig = static_cast<uint16_t>(static_cast<uint16_t>(FDC1004Rate::SPS_100) << 10);
  uint8_t _capdacCodes[4] = {0, 0, 0, 0};
  bool _capdacEnabled[4] = {false, false, false, false};
  FDC1004Channel _measurementChannels[4] = {FDC1004Channel::CIN1, FDC1004Channel::CIN1, FDC1004Channel::CIN1, FDC1004Channel::CIN1};
  bool _measurementConfigured[4] = {false, false, false, false};
  FDCxStatus _lastStatus = FDCxStatus::DeviceNotFound;
};
