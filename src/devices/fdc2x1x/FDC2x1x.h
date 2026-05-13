#pragma once

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>
#include "../../fdcx/FDCxI2C.h"
#include "../../fdcx/FDCxStatus.h"
#include "../../core/FDCxCapability.h"
#include "FDC2x1xTypes.h"
#include "FDC2x1xRegisters.h"
#include "FDC2x1xFrequency.h"
#include "FDC2x1xDerivedCapacitance.h"
#include "FDC2x1xResonator.h"

#ifndef FDCX_DEPRECATED
// Arduino-core compatibility note: deprecation warnings are opt-in so older embedded
// toolchains are not broken by -Werror. Define FDCX_ENABLE_DEPRECATED_WARNINGS to
// emit compiler deprecation warnings for legacy compatibility overloads.
#if defined(FDCX_ENABLE_DEPRECATED_WARNINGS)
#define FDCX_DEPRECATED(msg) [[deprecated(msg)]]
#else
#define FDCX_DEPRECATED(msg)
#endif
#endif

class FDC2x1x {
 public:
  FDC2x1x() = default;
  FDCxStatus begin(TwoWire& wire, FDC2x1xVariant variant = FDC2x1xVariant::FDC2214,
                   uint8_t address = fdc2x1x_registers::kDefaultI2cAddressLow);
  FDCxStatus begin(FDCxI2C& bus, FDC2x1xVariant variant = FDC2x1xVariant::FDC2214,
                   uint8_t address = fdc2x1x_registers::kDefaultI2cAddressLow);
  FDCxStatus readRegister16(uint8_t reg, uint16_t& value);
  FDCxStatus writeRegister16(uint8_t reg, uint16_t value);
  FDCxStatus validateChannel(FDC2x1xChannel channel);
  FDCxStatus configureChannel(FDC2x1xChannel channel);  // Compatibility alias: validation-only.
  FDCxStatus setRCount(FDC2x1xChannel channel, uint16_t value);
  FDCxStatus setSettleCount(FDC2x1xChannel channel, uint16_t value);
  FDCxStatus configureClockDividers(FDC2x1xChannel channel, uint8_t finSelMultiplier, uint16_t frefDivider);
  FDCxStatus configureClockDividers(FDC2x1xChannel channel, FDC2x1xSensorTopology topology, uint16_t frefDivider);
  FDCxStatus configureClockDividersForSingleEnded(FDC2x1xChannel channel, uint16_t frefDivider);
  FDCxStatus configureClockDividersForDifferential(FDC2x1xChannel channel, uint8_t finSelMultiplier, uint16_t frefDivider);
  FDCxStatus writeRawClockDividers(FDC2x1xChannel channel, uint16_t rawValue);
  FDCxStatus writeRawRCount(FDC2x1xChannel channel, uint16_t rawValue);
  FDCxStatus writeRawSettleCount(FDC2x1xChannel channel, uint16_t rawValue);
  FDCxStatus writeRawDriveCurrent(FDC2x1xChannel channel, uint16_t rawValue);
  FDCxStatus setDriveCurrentCode(FDC2x1xChannel channel, uint8_t code5bit);
  FDCxStatus setClockDividers(FDC2x1xChannel channel, uint16_t value);  // Compatibility setter; Sleep-mode guarded. Use writeRawClockDividers() for explicit raw access.
  FDCxStatus setDriveCurrent(FDC2x1xChannel channel, uint16_t value);
  FDCxStatus setDeglitch(FDC2x1xDeglitch deglitch);
  FDCxStatus setReferenceClockSource(FDC2x1xReferenceClockSource source);
  FDCxStatus referenceClockSource(FDC2x1xReferenceClockSource& source);
  bool isSleepModeEnabled();
  FDCxStatus setSleepMode(bool enabled);
  FDCxStatus setActiveChannel(FDC2x1xChannel channel);
  FDCxStatus startContinuous(FDC2x1xChannel channel);
  FDCxStatus stopConversions();
  FDCxStatus configureSingleChannel(FDC2x1xChannel channel,
                                    uint16_t rcount,
                                    uint16_t settleCount,
                                    uint8_t finSelMultiplier,
                                    uint16_t frefDivider,
                                    uint8_t driveCurrentCode,
                                    FDC2x1xDeglitch deglitch);
  FDCxStatus configureSingleChannel(FDC2x1xChannel channel,
                                    uint16_t rcount,
                                    uint16_t settleCount,
                                    FDC2x1xSensorTopology topology,
                                    uint16_t frefDivider,
                                    uint8_t driveCurrentCode,
                                    FDC2x1xDeglitch deglitch);
  FDCxStatus waitDataReady(FDC2x1xChannel channel, uint16_t timeoutMs);
  FDCxStatus readRaw(FDC2x1xChannel channel, uint32_t& rawCode);
  // Legacy-only compatibility helper. Not recommended for real hardware.
  // Prefer readFrequencyHzFromRegisters(channel, fclkHz, output), which decodes
  // CHx_FIN_SEL and CHx_FREF_DIVIDER from CLOCK_DIVIDERS_CHx.
  // Define FDCX_ENABLE_DEPRECATED_WARNINGS to get a compiler warning for this overload.
  FDCX_DEPRECATED("Use readFrequencyHzFromRegisters(channel, fclkHz, output) or the explicit FIN_SEL overload.")
  FDCxStatus readFrequencyHz(FDC2x1xChannel channel, float frefHz, FDC2x1xFrequency& output);
  FDCxStatus readFrequencyHz(FDC2x1xChannel channel, float frefHz, uint8_t finSelMultiplier, FDC2x1xFrequency& output);
  FDCxStatus readFrequencyHzFromRegisters(FDC2x1xChannel channel, float fclkHz, FDC2x1xFrequency& output);
  FDCxStatus deriveCapacitancePf(const FDC2x1xFrequency& frequency,
                                 const FDC2x1xLCParams& params,
                                 FDC2x1xDerivedCapacitance& output);
  FDCxCapability capability() const;
  FDCxStatus idle();
  FDC2x1xVariant variant() const { return _variant; }
  uint8_t address() const { return _address; }
  FDCxStatus lastStatus() const { return _lastStatus; }
  bool isFDC211x() const { return fdc2x1xVariantIsFdc211x(_variant); }
  bool isFDC221x() const { return fdc2x1xVariantIsFdc221x(_variant); }

 private:
  bool channelValid(FDC2x1xChannel channel) const;
  bool registerAllowedForVariant(uint8_t reg, bool write) const;
  FDCxStatus writeConfigPreservingOptions(FDC2x1xChannel channel, bool sleepEnabled);
  FDCxStatus configureSingleChannelMux(FDC2x1xDeglitch deglitch);
  bool configWriteAllowedForVariant(uint16_t value) const;
  bool muxConfigWriteAllowedForVariant(uint16_t value) const;
  bool resetDevWriteAllowedForVariant(uint16_t value) const;
  FDCxStatus requireSleepModeForConfigWrite();
  FDCxStatus setStatus(FDCxStatus status);

  FDCxTwoWireI2C _wireAdapter;
  FDCxI2C* _bus = nullptr;
  FDC2x1xVariant _variant = FDC2x1xVariant::FDC2214;
  uint8_t _address = fdc2x1x_registers::kDefaultI2cAddressLow;
  uint16_t _timeoutMs = 100;
  FDCxStatus _lastStatus = FDCxStatus::DeviceNotFound;
};
