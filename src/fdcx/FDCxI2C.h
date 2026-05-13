#pragma once

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include "FDCxStatus.h"

class FDCxI2C {
 public:
  virtual ~FDCxI2C() = default;
  virtual FDCxStatus readRegister16(uint8_t address, uint8_t reg, uint16_t& value) = 0;
  virtual FDCxStatus writeRegister16(uint8_t address, uint8_t reg, uint16_t value) = 0;
};

class FDCxTwoWireI2C final : public FDCxI2C {
 public:
  FDCxTwoWireI2C() = default;
  explicit FDCxTwoWireI2C(TwoWire& wire) : _wire(&wire) {}

  void attach(TwoWire& wire) {
    _wire = &wire;
  }

  FDCxStatus readRegister16(uint8_t address, uint8_t reg, uint16_t& value) override {
    if (_wire == nullptr) {
      return FDCxStatus::I2cError;
    }

    _wire->beginTransmission(address);
    if (_wire->write(reg) != 1) {
      _wire->endTransmission();
      return FDCxStatus::I2cError;
    }

    const uint8_t txStatus = _wire->endTransmission(false);
    if (txStatus != 0) {
      return FDCxStatus::I2cError;
    }

    const uint8_t requested = 2;
    const uint8_t received = static_cast<uint8_t>(_wire->requestFrom(address, requested));
    if (received != requested || _wire->available() < requested) {
      return FDCxStatus::I2cError;
    }

    const uint16_t high = static_cast<uint16_t>(_wire->read()) & 0x00FFu;
    const uint16_t low = static_cast<uint16_t>(_wire->read()) & 0x00FFu;
    value = static_cast<uint16_t>((high << 8) | low);
    return FDCxStatus::Ok;
  }

  FDCxStatus writeRegister16(uint8_t address, uint8_t reg, uint16_t value) override {
    if (_wire == nullptr) {
      return FDCxStatus::I2cError;
    }

    _wire->beginTransmission(address);
    if (_wire->write(reg) != 1) {
      _wire->endTransmission();
      return FDCxStatus::I2cError;
    }
    if (_wire->write(static_cast<uint8_t>((value >> 8) & 0xFFu)) != 1) {
      _wire->endTransmission();
      return FDCxStatus::I2cError;
    }
    if (_wire->write(static_cast<uint8_t>(value & 0xFFu)) != 1) {
      _wire->endTransmission();
      return FDCxStatus::I2cError;
    }

    const uint8_t txStatus = _wire->endTransmission();
    return txStatus == 0 ? FDCxStatus::Ok : FDCxStatus::I2cError;
  }

 private:
  TwoWire* _wire = nullptr;
};
