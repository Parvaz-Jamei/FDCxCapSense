#include "FDC2x1x.h"
#include "FDC2x1xConversion.h"

namespace {
uint16_t expectedDeviceIdForVariant(FDC2x1xVariant variant) {
  return fdc2x1xVariantIsFdc211x(variant) ? fdc2x1x_registers::kExpectedDeviceIdFdc211x
                                          : fdc2x1x_registers::kExpectedDeviceIdFdc221x;
}

bool addressIsSupported(uint8_t address) {
  return address == fdc2x1x_registers::kDefaultI2cAddressLow ||
         address == fdc2x1x_registers::kDefaultI2cAddressHigh;
}

bool finSelMultiplierIsValid(uint8_t multiplier) {
  return multiplier == 1u || multiplier == 2u;
}
}  // namespace

FDCxStatus FDC2x1x::begin(TwoWire& wire, FDC2x1xVariant variant, uint8_t address) {
  _wireAdapter.attach(wire);
  return begin(_wireAdapter, variant, address);
}

FDCxStatus FDC2x1x::begin(FDCxI2C& bus, FDC2x1xVariant variant, uint8_t address) {
  if (!addressIsSupported(address) || !fdc2x1xVariantIsKnown(variant)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  _bus = &bus;
  _variant = variant;
  _address = address;

  uint16_t manufacturer = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::kManufacturerId, manufacturer);
  if (st != FDCxStatus::Ok) {
    return setStatus(FDCxStatus::DeviceNotFound);
  }
  if (manufacturer != fdc2x1x_registers::kExpectedManufacturerId) {
    return setStatus(FDCxStatus::InvalidDeviceId);
  }

  uint16_t device = 0;
  st = readRegister16(fdc2x1x_registers::kDeviceId, device);
  if (st != FDCxStatus::Ok) {
    return st;
  }
  if (device != expectedDeviceIdForVariant(variant)) {
    return setStatus(FDCxStatus::InvalidDeviceId);
  }
  // Device ID validates only the FDC211x vs FDC221x family. The exact 2-channel
  // vs 4-channel variant is user-declared from the hardware BOM. Channel guards
  // therefore use _variant, not an inferred channel count from DEVICE_ID.
  return setStatus(FDCxStatus::Ok);
}

FDCxStatus FDC2x1x::readRegister16(uint8_t reg, uint16_t& value) {
  if (_bus == nullptr) return setStatus(FDCxStatus::DeviceNotFound);
  if (!fdc2x1x_registers::isKnownReadableRegister(reg) || !registerAllowedForVariant(reg, false)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return setStatus(_bus->readRegister16(_address, reg, value));
}

FDCxStatus FDC2x1x::writeRegister16(uint8_t reg, uint16_t value) {
  if (_bus == nullptr) return setStatus(FDCxStatus::DeviceNotFound);
  if (!fdc2x1x_registers::isKnownWritableRegister(reg) || !registerAllowedForVariant(reg, true)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  if (reg == fdc2x1x_registers::kConfig) {
    if (!fdc2x1x_registers::configReservedPatternIsValid(value) || !configWriteAllowedForVariant(value)) {
      return setStatus(FDCxStatus::InvalidArgument);
    }
    if ((value & fdc2x1x_registers::kConfigHighCurrentMask) != 0u) {
      uint16_t currentMux = 0;
      const FDCxStatus st = readRegister16(fdc2x1x_registers::kMuxConfig, currentMux);
      if (st != FDCxStatus::Ok) return st;
      if (fdc2x1x_registers::muxAutoscanEnabled(currentMux)) {
        return setStatus(FDCxStatus::InvalidArgument);
      }
    }
  }
  if (reg == fdc2x1x_registers::kMuxConfig) {
    if (!fdc2x1x_registers::muxConfigReservedPatternIsValid(value) || !muxConfigWriteAllowedForVariant(value)) {
      return setStatus(FDCxStatus::InvalidArgument);
    }
    if (fdc2x1x_registers::muxAutoscanEnabled(value)) {
      uint16_t currentConfig = 0;
      const FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, currentConfig);
      if (st != FDCxStatus::Ok) return st;
      if ((currentConfig & fdc2x1x_registers::kConfigHighCurrentMask) != 0u) {
        return setStatus(FDCxStatus::InvalidArgument);
      }
    }
  }
  if (reg == fdc2x1x_registers::kErrorConfig) {
    if ((value & static_cast<uint16_t>(~fdc2x1x_registers::kErrorConfigAllowedMask)) != 0u) {
      return setStatus(FDCxStatus::InvalidArgument);
    }
  }
  if (reg == fdc2x1x_registers::kResetDev) {
    if (!resetDevWriteAllowedForVariant(value)) {
      return setStatus(FDCxStatus::InvalidArgument);
    }
  }
  return setStatus(_bus->writeRegister16(_address, reg, value));
}

bool FDC2x1x::channelValid(FDC2x1xChannel channel) const {
  return fdc2x1xChannelIsValid(_variant, channel);
}

bool FDC2x1x::registerAllowedForVariant(uint8_t reg, bool write) const {
  if (!fdc2x1xVariantIsKnown(_variant)) return false;
  if (fdc2x1x_registers::isDataRegister(reg)) {
    const uint8_t channel = static_cast<uint8_t>(reg / 2u);
    if (channel >= fdc2x1xVariantChannelCount(_variant)) return false;
    if (write) return false;
    if (isFDC211x() && (reg & 0x01u) != 0u) return false;  // DATA_LSB_CHx is not used by FDC211x.
    return true;
  }

  uint8_t channel = 0;
  if (fdc2x1x_registers::isChannelRegister(reg, fdc2x1x_registers::kRCountCh0, channel) ||
      fdc2x1x_registers::isChannelRegister(reg, fdc2x1x_registers::kSettleCountCh0, channel) ||
      fdc2x1x_registers::isChannelRegister(reg, fdc2x1x_registers::kClockDividersCh0, channel) ||
      fdc2x1x_registers::isChannelRegister(reg, fdc2x1x_registers::kDriveCurrentCh0, channel)) {
    return channel < fdc2x1xVariantChannelCount(_variant);
  }
  if (fdc2x1x_registers::isChannelRegister(reg, fdc2x1x_registers::kOffsetCh0, channel)) {
    return isFDC211x() && channel < fdc2x1xVariantChannelCount(_variant);
  }
  return true;
}

bool FDC2x1x::configWriteAllowedForVariant(uint16_t value) const {
  const uint8_t active = fdc2x1x_registers::activeChannelFromConfig(value);
  if (active >= fdc2x1xVariantChannelCount(_variant)) {
    return false;
  }
  // TI specifies HIGH_CURRENT_DRV for CH0 in single-channel mode only.
  if ((value & fdc2x1x_registers::kConfigHighCurrentMask) != 0u && active != 0u) {
    return false;
  }
  return true;
}

bool FDC2x1x::muxConfigWriteAllowedForVariant(uint16_t value) const {
  if (!fdc2x1x_registers::muxAutoscanEnabled(value)) {
    return true;
  }
  const uint8_t rrSequence = fdc2x1x_registers::rrSequenceFromMuxConfig(value);
  if (fdc2x1xVariantChannelCount(_variant) <= 2u) {
    // b00 and b11 both select CH0/CH1. b01 and b10 include CH2/CH3 and are 4-channel-only.
    return rrSequence == 0u || rrSequence == 3u;
  }
  return rrSequence <= 3u;
}

bool FDC2x1x::resetDevWriteAllowedForVariant(uint16_t value) const {
  const uint16_t allowed = isFDC211x() ? fdc2x1x_registers::kResetDevFdc211xAllowedMask
                                      : fdc2x1x_registers::kResetDevFdc221xAllowedMask;
  return (value & static_cast<uint16_t>(~allowed)) == 0u;
}

bool FDC2x1x::isSleepModeEnabled() {
  uint16_t config = 0;
  const FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, config);
  if (st != FDCxStatus::Ok) return false;
  return (config & fdc2x1x_registers::kConfigSleepModeMask) != 0u;
}

FDCxStatus FDC2x1x::requireSleepModeForConfigWrite() {
  uint16_t config = 0;
  const FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, config);
  if (st != FDCxStatus::Ok) return st;
  if ((config & fdc2x1x_registers::kConfigSleepModeMask) == 0u) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return setStatus(FDCxStatus::Ok);
}

FDCxStatus FDC2x1x::validateChannel(FDC2x1xChannel channel) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  return setStatus(FDCxStatus::Ok);
}

FDCxStatus FDC2x1x::configureChannel(FDC2x1xChannel channel) {
  // Compatibility alias retained for existing sketches. This validates the user-declared variant channel only.
  // Real hardware startup must configure timing/current/deglitch and then call startContinuous().
  return validateChannel(channel);
}

FDCxStatus FDC2x1x::writeRawRCount(FDC2x1xChannel channel, uint16_t rawValue) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  return writeRegister16(fdc2x1x_registers::rcountRegister(static_cast<uint8_t>(channel)), rawValue);
}

FDCxStatus FDC2x1x::writeRawSettleCount(FDC2x1xChannel channel, uint16_t rawValue) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  return writeRegister16(fdc2x1x_registers::settleCountRegister(static_cast<uint8_t>(channel)), rawValue);
}

FDCxStatus FDC2x1x::writeRawClockDividers(FDC2x1xChannel channel, uint16_t rawValue) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  if ((rawValue & ~fdc2x1x_registers::kClockDividerAllowedMask) != 0u) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  uint8_t decodedFinSel = 0;
  if (!fdc2x1x_registers::decodeFinSelMultiplier(rawValue, decodedFinSel) ||
      fdc2x1x_registers::decodeFrefDivider(rawValue) == 0u) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return writeRegister16(fdc2x1x_registers::clockDividersRegister(static_cast<uint8_t>(channel)), rawValue);
}

FDCxStatus FDC2x1x::writeRawDriveCurrent(FDC2x1xChannel channel, uint16_t rawValue) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  if ((rawValue & ~fdc2x1x_registers::kDriveCurrentCodeMask) != 0u) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return writeRegister16(fdc2x1x_registers::driveCurrentRegister(static_cast<uint8_t>(channel)), rawValue);
}

FDCxStatus FDC2x1x::setDriveCurrentCode(FDC2x1xChannel channel, uint8_t code5bit) {
  if (code5bit > 31u) return setStatus(FDCxStatus::InvalidArgument);
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  return writeRawDriveCurrent(channel, static_cast<uint16_t>(static_cast<uint16_t>(code5bit) << 11));
}

FDCxStatus FDC2x1x::setRCount(FDC2x1xChannel channel, uint16_t value) {
  if (value < 0x0100u) return setStatus(FDCxStatus::InvalidArgument);
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  return writeRawRCount(channel, value);
}
FDCxStatus FDC2x1x::setSettleCount(FDC2x1xChannel channel, uint16_t value) {
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  return writeRawSettleCount(channel, value);
}
FDCxStatus FDC2x1x::setDriveCurrent(FDC2x1xChannel channel, uint16_t value) {
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  return writeRawDriveCurrent(channel, value);
}

FDCxStatus FDC2x1x::configureClockDividers(FDC2x1xChannel channel, uint8_t finSelMultiplier, uint16_t frefDivider) {
  if (!channelValid(channel) || !finSelMultiplierIsValid(finSelMultiplier) || frefDivider == 0u ||
      (frefDivider & ~fdc2x1x_registers::kClockFrefDividerMask) != 0u) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  const uint16_t value = static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(finSelMultiplier) |
                                               (frefDivider & fdc2x1x_registers::kClockFrefDividerMask));
  return writeRawClockDividers(channel, value);
}

FDCxStatus FDC2x1x::configureClockDividers(FDC2x1xChannel channel, FDC2x1xSensorTopology topology, uint16_t frefDivider) {
  if (topology == FDC2x1xSensorTopology::SingleEnded) {
    return configureClockDividers(channel, 2u, frefDivider);
  }
  if (topology == FDC2x1xSensorTopology::Differential) {
    // Differential sensors can require FIN_SEL=1 or FIN_SEL=2 depending on frequency range.
    // Use configureClockDividersForDifferential() to choose the multiplier explicitly.
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return setStatus(FDCxStatus::InvalidArgument);
}

FDCxStatus FDC2x1x::configureClockDividersForSingleEnded(FDC2x1xChannel channel, uint16_t frefDivider) {
  return configureClockDividers(channel, FDC2x1xSensorTopology::SingleEnded, frefDivider);
}

FDCxStatus FDC2x1x::configureClockDividersForDifferential(FDC2x1xChannel channel, uint8_t finSelMultiplier, uint16_t frefDivider) {
  if (!finSelMultiplierIsValid(finSelMultiplier)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return configureClockDividers(channel, finSelMultiplier, frefDivider);
}

FDCxStatus FDC2x1x::setClockDividers(FDC2x1xChannel channel, uint16_t value) {
  // Compatibility setter kept for existing sketches; unlike the explicit raw path,
  // this public setter is Sleep-mode guarded for industrial-safe configuration writes.
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  return writeRawClockDividers(channel, value);
}

FDCxStatus FDC2x1x::setDeglitch(FDC2x1xDeglitch deglitch) {
  if (deglitch != FDC2x1xDeglitch::MHz_1 && deglitch != FDC2x1xDeglitch::MHz_3_3 &&
      deglitch != FDC2x1xDeglitch::MHz_10 && deglitch != FDC2x1xDeglitch::MHz_33) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  uint16_t mux = 0;
  st = readRegister16(fdc2x1x_registers::kMuxConfig, mux);
  if (st != FDCxStatus::Ok) {
    return st;
  }
  mux = static_cast<uint16_t>((mux & fdc2x1x_registers::kMuxConfigAutoscanSequenceMask) |
                              fdc2x1x_registers::kMuxConfigReservedPattern |
                              (static_cast<uint16_t>(deglitch) & fdc2x1x_registers::kMuxConfigDeglitchMask));
  return writeRegister16(fdc2x1x_registers::kMuxConfig, mux);
}

FDCxStatus FDC2x1x::setReferenceClockSource(FDC2x1xReferenceClockSource source) {
  if (source != FDC2x1xReferenceClockSource::InternalOscillator &&
      source != FDC2x1xReferenceClockSource::ExternalClockInput) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  FDCxStatus st = requireSleepModeForConfigWrite();
  if (st != FDCxStatus::Ok) return st;
  uint16_t config = 0;
  st = readRegister16(fdc2x1x_registers::kConfig, config);
  if (st != FDCxStatus::Ok) return st;
  if (!configWriteAllowedForVariant(config)) return setStatus(FDCxStatus::InvalidArgument);
  if (source == FDC2x1xReferenceClockSource::ExternalClockInput) {
    config = static_cast<uint16_t>(config | fdc2x1x_registers::kConfigRefClockSrcMask);
  } else {
    config = static_cast<uint16_t>(config & ~fdc2x1x_registers::kConfigRefClockSrcMask);
  }
  // Re-force reserved bits without disturbing the real CONFIG option fields.
  config = static_cast<uint16_t>((config & ~fdc2x1x_registers::kConfigReservedMask) |
                                 fdc2x1x_registers::kConfigReservedRequiredOnes);
  return writeRegister16(fdc2x1x_registers::kConfig, config);
}

FDCxStatus FDC2x1x::referenceClockSource(FDC2x1xReferenceClockSource& source) {
  uint16_t config = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, config);
  if (st != FDCxStatus::Ok) return st;
  source = ((config & fdc2x1x_registers::kConfigRefClockSrcMask) != 0u)
               ? FDC2x1xReferenceClockSource::ExternalClockInput
               : FDC2x1xReferenceClockSource::InternalOscillator;
  return setStatus(FDCxStatus::Ok);
}

FDCxStatus FDC2x1x::writeConfigPreservingOptions(FDC2x1xChannel channel, bool sleepEnabled) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  uint16_t oldConfig = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, oldConfig);
  if (st != FDCxStatus::Ok) return st;
  const uint16_t preservedOptions = oldConfig & fdc2x1x_registers::kConfigOptionPreserveMask;
  const uint16_t value = fdc2x1x_registers::configValueFor(channel, sleepEnabled, preservedOptions);
  return writeRegister16(fdc2x1x_registers::kConfig, value);
}

FDCxStatus FDC2x1x::configureSingleChannelMux(FDC2x1xDeglitch deglitch) {
  if (deglitch != FDC2x1xDeglitch::MHz_1 && deglitch != FDC2x1xDeglitch::MHz_3_3 &&
      deglitch != FDC2x1xDeglitch::MHz_10 && deglitch != FDC2x1xDeglitch::MHz_33) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return writeRegister16(fdc2x1x_registers::kMuxConfig, fdc2x1x_registers::singleChannelMuxValue(deglitch));
}

FDCxStatus FDC2x1x::setSleepMode(bool enabled) {
  uint16_t config = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, config);
  if (st != FDCxStatus::Ok) return st;
  const uint8_t active = fdc2x1x_registers::activeChannelFromConfig(config);
  if (active >= fdc2x1xVariantChannelCount(_variant)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return writeConfigPreservingOptions(static_cast<FDC2x1xChannel>(active), enabled);
}

FDCxStatus FDC2x1x::setActiveChannel(FDC2x1xChannel channel) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  uint16_t config = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, config);
  if (st != FDCxStatus::Ok) return st;
  if ((config & fdc2x1x_registers::kConfigSleepModeMask) == 0u) {
    // Industrial-safe public path: do not switch ACTIVE_CHAN while conversions are running.
    // Use startContinuous(newChannel), which enters Sleep Mode before changing the channel.
    return setStatus(FDCxStatus::InvalidArgument);
  }
  return writeConfigPreservingOptions(channel, true);
}

FDCxStatus FDC2x1x::startContinuous(FDC2x1xChannel channel) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  uint16_t config = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::kConfig, config);
  if (st != FDCxStatus::Ok) return st;
  const uint8_t currentActive = fdc2x1x_registers::activeChannelFromConfig(config);
  if (currentActive >= fdc2x1xVariantChannelCount(_variant)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  const bool sleepEnabled = (config & fdc2x1x_registers::kConfigSleepModeMask) != 0u;
  if ((config & fdc2x1x_registers::kConfigHighCurrentMask) != 0u &&
      channel != FDC2x1xChannel::CH0) {
    // Preflight before entering Sleep Mode: HIGH_CURRENT_DRV is documented for
    // CH0 single-channel mode only, so a rejected CH1/CH2/CH3 request must not
    // mutate device state.
    return setStatus(FDCxStatus::InvalidArgument);
  }
  if (!sleepEnabled) {
    if (currentActive == static_cast<uint8_t>(channel)) {
      // Already running this channel; keep the active state and preserve CONFIG options.
      return writeConfigPreservingOptions(channel, false);
    }
    // Safe channel switch: enter Sleep Mode before changing ACTIVE_CHAN, then restart.
    st = writeConfigPreservingOptions(static_cast<FDC2x1xChannel>(currentActive), true);
    if (st != FDCxStatus::Ok) return st;
  }
  st = setActiveChannel(channel);
  if (st != FDCxStatus::Ok) return st;
  return writeConfigPreservingOptions(channel, false);
}

FDCxStatus FDC2x1x::stopConversions() {
  return setSleepMode(true);
}

FDCxStatus FDC2x1x::configureSingleChannel(FDC2x1xChannel channel,
                                            uint16_t rcount,
                                            uint16_t settleCount,
                                            uint8_t finSelMultiplier,
                                            uint16_t frefDivider,
                                            uint8_t driveCurrentCode,
                                            FDC2x1xDeglitch deglitch) {
  if (!channelValid(channel) || rcount < 0x0100u || !finSelMultiplierIsValid(finSelMultiplier) ||
      frefDivider == 0u || (frefDivider & ~fdc2x1x_registers::kClockFrefDividerMask) != 0u ||
      driveCurrentCode > 31u ||
      (deglitch != FDC2x1xDeglitch::MHz_1 && deglitch != FDC2x1xDeglitch::MHz_3_3 &&
       deglitch != FDC2x1xDeglitch::MHz_10 && deglitch != FDC2x1xDeglitch::MHz_33)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  FDCxStatus st = setSleepMode(true);
  if (st != FDCxStatus::Ok) return st;
  st = setRCount(channel, rcount);
  if (st != FDCxStatus::Ok) return st;
  st = setSettleCount(channel, settleCount);
  if (st != FDCxStatus::Ok) return st;
  st = configureClockDividers(channel, finSelMultiplier, frefDivider);
  if (st != FDCxStatus::Ok) return st;
  st = setDriveCurrentCode(channel, driveCurrentCode);
  if (st != FDCxStatus::Ok) return st;
  st = configureSingleChannelMux(deglitch);
  if (st != FDCxStatus::Ok) return st;
  return startContinuous(channel);
}

FDCxStatus FDC2x1x::configureSingleChannel(FDC2x1xChannel channel,
                                            uint16_t rcount,
                                            uint16_t settleCount,
                                            FDC2x1xSensorTopology topology,
                                            uint16_t frefDivider,
                                            uint8_t driveCurrentCode,
                                            FDC2x1xDeglitch deglitch) {
  if (topology == FDC2x1xSensorTopology::SingleEnded) {
    return configureSingleChannel(channel, rcount, settleCount, 2u, frefDivider, driveCurrentCode, deglitch);
  }
  // Differential sensors need an explicit FIN_SEL choice. Use the finSelMultiplier overload.
  return setStatus(FDCxStatus::InvalidArgument);
}

FDCxStatus FDC2x1x::waitDataReady(FDC2x1xChannel channel, uint16_t timeoutMs) {
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  const uint16_t channelUnreadMask = fdc2x1x_registers::unreadMaskForChannel(channel);
  if (channelUnreadMask == 0u) return setStatus(FDCxStatus::InvalidArgument);
  const unsigned long start = millis();
  do {
    uint16_t status = 0;
    const FDCxStatus readStatus = readRegister16(fdc2x1x_registers::kStatus, status);
    if (readStatus != FDCxStatus::Ok) return readStatus;
    if ((status & fdc2x1x_registers::kStatusErrorMask) != 0u) {
      return setStatus(FDCxStatus::MeasurementNotReady);
    }
    if ((status & channelUnreadMask) != 0u) {
      return setStatus(FDCxStatus::Ok);
    }
    delay(1);
  } while (static_cast<unsigned long>(millis() - start) < timeoutMs);
  return setStatus(FDCxStatus::Timeout);
}

FDCxStatus FDC2x1x::readRaw(FDC2x1xChannel channel, uint32_t& rawCode) {
  rawCode = 0;
  if (!channelValid(channel)) return setStatus(FDCxStatus::InvalidArgument);
  uint16_t msb = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::dataMsbRegister(static_cast<uint8_t>(channel)), msb);
  if (st != FDCxStatus::Ok) return st;
  if ((msb & fdc2x1x_registers::kDataErrorMask) != 0u) {
    return setStatus(FDCxStatus::MeasurementNotReady);
  }
  if (isFDC221x()) {
    uint16_t lsb = 0;
    st = readRegister16(fdc2x1x_registers::dataLsbRegister(static_cast<uint8_t>(channel)), lsb);
    if (st != FDCxStatus::Ok) return st;
    rawCode = ((static_cast<uint32_t>(msb) & fdc2x1x_registers::kDataMsbMask) << 16) | static_cast<uint32_t>(lsb);
  } else {
    rawCode = static_cast<uint32_t>(msb & fdc2x1x_registers::kDataMsbMask);
  }
  return setStatus(FDCxStatus::Ok);
}

FDCxStatus FDC2x1x::readFrequencyHz(FDC2x1xChannel channel, float frefHz, FDC2x1xFrequency& output) {
  output = FDC2x1xFrequency();
  output.variant = _variant;
  output.channel = channel;
  output.frefHz = frefHz;
  if (!channelValid(channel) || !fdc2x1xIsFiniteFloat(frefHz) || frefHz <= 0.0f) {
    output.status = setStatus(FDCxStatus::InvalidArgument);
    return output.status;
  }
  if (isFDC211x()) {
    output.status = setStatus(FDCxStatus::NotSupported);
    output.dataReady = false;
    return output.status;
  }
  uint16_t clockReg = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::clockDividersRegister(static_cast<uint8_t>(channel)), clockReg);
  if (st != FDCxStatus::Ok) {
    output.status = st;
    return st;
  }
  uint8_t finSelMultiplier = 0;
  if (!fdc2x1x_registers::decodeFinSelMultiplier(clockReg, finSelMultiplier)) {
    output.status = setStatus(FDCxStatus::InvalidArgument);
    return output.status;
  }
  const uint16_t decodedFrefDivider = fdc2x1x_registers::decodeFrefDivider(clockReg);
  if (decodedFrefDivider == 0u) {
    output.status = setStatus(FDCxStatus::InvalidArgument);
    return output.status;
  }
  st = readFrequencyHz(channel, frefHz, finSelMultiplier, output);
  output.frefDivider = decodedFrefDivider;
  output.frefHz = frefHz;
  output.finSelMultiplier = finSelMultiplier;
  return st;
}

FDCxStatus FDC2x1x::readFrequencyHz(FDC2x1xChannel channel, float frefHz, uint8_t finSelMultiplier, FDC2x1xFrequency& output) {
  output = FDC2x1xFrequency();
  output.variant = _variant;
  output.channel = channel;
  output.frefHz = frefHz;
  output.finSelMultiplier = finSelMultiplier;
  output.frefDivider = 0;
  if (!channelValid(channel) || !fdc2x1xIsFiniteFloat(frefHz) || frefHz <= 0.0f || !finSelMultiplierIsValid(finSelMultiplier)) {
    output.status = setStatus(FDCxStatus::InvalidArgument);
    return output.status;
  }
  if (isFDC211x()) {
    output.status = setStatus(FDCxStatus::NotSupported);
    output.dataReady = false;
    return output.status;
  }

  FDCxStatus st = waitDataReady(channel, _timeoutMs);
  if (st != FDCxStatus::Ok) {
    output.status = st;
    output.dataReady = false;
    return st;
  }
  output.dataReady = true;

  uint32_t raw = 0;
  st = readRaw(channel, raw);
  output.rawCode = raw;
  if (st != FDCxStatus::Ok) {
    output.status = st;
    output.dataReady = false;
    return st;
  }
  output.frequencyHz = (static_cast<float>(finSelMultiplier) * frefHz * static_cast<float>(raw)) /
                       static_cast<float>(fdc2x1x_registers::kRawDenominator28);
  output.dataReady = true;
  output.status = setStatus(FDCxStatus::Ok);
  return output.status;
}

FDCxStatus FDC2x1x::readFrequencyHzFromRegisters(FDC2x1xChannel channel, float fclkHz, FDC2x1xFrequency& output) {
  output = FDC2x1xFrequency();
  output.variant = _variant;
  output.channel = channel;
  if (!channelValid(channel) || !fdc2x1xIsFiniteFloat(fclkHz) || fclkHz <= 0.0f) {
    output.status = setStatus(FDCxStatus::InvalidArgument);
    return output.status;
  }
  uint16_t clockReg = 0;
  FDCxStatus st = readRegister16(fdc2x1x_registers::clockDividersRegister(static_cast<uint8_t>(channel)), clockReg);
  if (st != FDCxStatus::Ok) {
    output.status = st;
    return st;
  }
  uint8_t finSelMultiplier = 0;
  if (!fdc2x1x_registers::decodeFinSelMultiplier(clockReg, finSelMultiplier)) {
    output.status = setStatus(FDCxStatus::InvalidArgument);
    return output.status;
  }
  const uint16_t frefDivider = fdc2x1x_registers::decodeFrefDivider(clockReg);
  if (frefDivider == 0u) {
    output.status = setStatus(FDCxStatus::InvalidArgument);
    return output.status;
  }
  const float frefHz = fclkHz / static_cast<float>(frefDivider);
  st = readFrequencyHz(channel, frefHz, finSelMultiplier, output);
  output.frefDivider = frefDivider;
  output.frefHz = frefHz;
  output.finSelMultiplier = finSelMultiplier;
  return st;
}

FDCxStatus FDC2x1x::deriveCapacitancePf(const FDC2x1xFrequency& frequency,
                                         const FDC2x1xLCParams& params,
                                         FDC2x1xDerivedCapacitance& output) {
  return setStatus(fdc2x1xDeriveCapacitancePf(frequency, params, output));
}

FDCxCapability FDC2x1x::capability() const {
  FDCxCapability caps;
  caps.supportsDirectCapacitance = false;
  caps.chipUsesResonantFrequency = true;
  caps.driverSupportsFrequencyConversion = isFDC221x();
  caps.supportsResonantFrequency = caps.driverSupportsFrequencyConversion;
  caps.supportsCapdac = false;
  caps.supportsRepeatMode = true;
  caps.supportsTemperatureInput = false;
  caps.chipHasIntbDrdy = true;
  caps.driverSupportsInterruptCallbacks = false;
  caps.supportsHardwareInterrupt = false;
  return caps;
}

FDCxStatus FDC2x1x::idle() {
  return stopConversions();
}

FDCxStatus FDC2x1x::setStatus(FDCxStatus status) {
  _lastStatus = status;
  return status;
}
