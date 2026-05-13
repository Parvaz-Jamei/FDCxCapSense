#include "FDC1004.h"

FDCxStatus FDC1004::begin(TwoWire& wire, uint8_t address) {
  _wireAdapter.attach(wire);
  return begin(_wireAdapter, address);
}

FDCxStatus FDC1004::begin(FDCxI2C& bus, uint8_t address) {
  if (address > 0x7Fu) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  _bus = &bus;
  _address = address;
  _fdcConfig = static_cast<uint16_t>(static_cast<uint16_t>(FDC1004Rate::SPS_100) << 10);
  for (uint8_t i = 0; i < 4; ++i) {
    _capdacCodes[i] = 0;
    _capdacEnabled[i] = false;
    _measurementChannels[i] = FDC1004Channel::CIN1;
    _measurementConfigured[i] = false;
  }

  return readIdentification();
}

FDCxStatus FDC1004::readRegister16(uint8_t reg, uint16_t& value) {
  if (_bus == nullptr) {
    return setStatus(FDCxStatus::DeviceNotFound);
  }
  if (!fdc1004_registers::isKnownReadableRegister(reg)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  const FDCxStatus status = _bus->readRegister16(_address, reg, value);
  return setStatus(status);
}

FDCxStatus FDC1004::writeRegister16(uint8_t reg, uint16_t value) {
  if (_bus == nullptr) {
    return setStatus(FDCxStatus::DeviceNotFound);
  }
  if (!fdc1004_registers::isKnownWritableRegister(reg)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  const FDCxStatus status = _bus->writeRegister16(_address, reg, value);
  if (status == FDCxStatus::Ok && reg == fdc1004_registers::kFdcConfig) {
    _fdcConfig = static_cast<uint16_t>(value & static_cast<uint16_t>(fdc1004_registers::kRateMask |
                                                                     fdc1004_registers::kRepeatBit |
                                                                     fdc1004_registers::kMeasurementTriggerMask));
  }
  return setStatus(status);
}

void FDC1004::setTimeoutMs(uint16_t timeoutMs) {
  _timeoutMs = timeoutMs;
}

FDCxStatus FDC1004::lastStatus() const {
  return _lastStatus;
}

FDCxCapability FDC1004::capability() const {
  FDCxCapability caps;
  caps.supportsDirectCapacitance = true;
  caps.supportsResonantFrequency = false;
  caps.supportsCapdac = true;
  caps.supportsRepeatMode = true;
  caps.supportsTemperatureInput = false;
  caps.chipHasIntbDrdy = false;
  caps.driverSupportsInterruptCallbacks = false;
  caps.supportsHardwareInterrupt = false;
  return caps;
}

FDCxStatus FDC1004::configureSingleEnded(FDC1004Measurement measurement,
                                         FDC1004Channel channel) {
  return configureSingleEndedInternal(measurement, channel, FDC1004CapdacMode::Disabled, 0);
}

FDCxStatus FDC1004::configureSingleEndedWithCapdac(FDC1004Measurement measurement,
                                                   FDC1004Channel channel,
                                                   uint8_t capdacCode) {
  return configureSingleEndedInternal(measurement, channel, FDC1004CapdacMode::Enabled, capdacCode);
}

FDCxStatus FDC1004::configureSingleEnded(FDC1004Measurement measurement,
                                         FDC1004Channel channel,
                                         uint8_t capdacCode) {
  return configureSingleEndedWithCapdac(measurement, channel, capdacCode);
}

FDCxStatus FDC1004::configureSingleEndedInternal(FDC1004Measurement measurement,
                                                 FDC1004Channel channel,
                                                 FDC1004CapdacMode mode,
                                                 uint8_t capdacCode) {
  if (!isValidMeasurement(measurement) || !isValidChannel(channel)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  if (mode == FDC1004CapdacMode::Disabled && capdacCode != 0) {
    return setStatus(FDCxStatus::InvalidArgument);
  }
  if (mode == FDC1004CapdacMode::Enabled && capdacCode > fdc1004_registers::kMaxCapdacCode) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  FDCxStatus encodingStatus = FDCxStatus::Ok;
  const uint16_t config = buildSingleEndedConfig(channel, mode, capdacCode, &encodingStatus);
  if (encodingStatus != FDCxStatus::Ok) {
    return setStatus(encodingStatus);
  }

  const FDCxStatus writeStatus = writeRegister16(measurementConfigRegister(measurement), config);
  if (writeStatus == FDCxStatus::Ok) {
    const uint8_t index = measurementIndex(measurement);
    _capdacCodes[index] = capdacCode;
    _capdacEnabled[index] = (mode == FDC1004CapdacMode::Enabled);
    _measurementChannels[index] = channel;
    _measurementConfigured[index] = true;
  }
  return writeStatus;
}

FDCxStatus FDC1004::setSampleRate(FDC1004Rate rate) {
  if (rate != FDC1004Rate::SPS_100 && rate != FDC1004Rate::SPS_200 && rate != FDC1004Rate::SPS_400) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  const uint16_t newConfig = static_cast<uint16_t>((_fdcConfig & ~fdc1004_registers::kRateMask) |
                                                   (static_cast<uint16_t>(rate) << 10));
  return writeRegister16(fdc1004_registers::kFdcConfig, newConfig);
}


FDCxStatus FDC1004::setRepeatMode(bool enabled) {
  const uint16_t repeatBit = enabled ? fdc1004_registers::kRepeatBit : 0u;
  const uint16_t newConfig = static_cast<uint16_t>((_fdcConfig & ~fdc1004_registers::kRepeatBit) | repeatBit);
  return writeRegister16(fdc1004_registers::kFdcConfig, newConfig);
}

FDCxStatus FDC1004::stopRepeatedMeasurements() {
  return setRepeatMode(false);
}

FDCxStatus FDC1004::idle() {
  const uint16_t newConfig = static_cast<uint16_t>(_fdcConfig & ~(fdc1004_registers::kRepeatBit |
                                                                  fdc1004_registers::kMeasurementTriggerMask));
  return writeRegister16(fdc1004_registers::kFdcConfig, newConfig);
}

FDCxStatus FDC1004::triggerSingleMeasurement(FDC1004Measurement measurement) {
  if (!isValidMeasurement(measurement)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  const uint16_t config = static_cast<uint16_t>((_fdcConfig & ~(fdc1004_registers::kRepeatBit |
                                                              fdc1004_registers::kMeasurementTriggerMask)) |
                                               measurementTriggerBit(measurement));
  return writeRegister16(fdc1004_registers::kFdcConfig, config);
}

FDCxStatus FDC1004::waitForMeasurement(FDC1004Measurement measurement, uint16_t timeoutMs) {
  if (!isValidMeasurement(measurement)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  const unsigned long start = millis();
  const uint16_t doneBit = measurementDoneBit(measurement);

  do {
    uint16_t config = 0;
    const FDCxStatus readStatus = readRegister16(fdc1004_registers::kFdcConfig, config);
    if (readStatus != FDCxStatus::Ok) {
      return readStatus;
    }
    if ((config & doneBit) != 0u) {
      return setStatus(FDCxStatus::Ok);
    }
    delay(1);
  } while (static_cast<unsigned long>(millis() - start) < timeoutMs);

  return setStatus(FDCxStatus::Timeout);
}

FDCxStatus FDC1004::readRaw24(FDC1004Measurement measurement, int32_t& raw24) {
  raw24 = 0;
  if (!isValidMeasurement(measurement)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  uint16_t msb = 0;
  uint16_t lsb = 0;
  FDCxStatus status = readRegister16(measurementMsbRegister(measurement), msb);
  if (status != FDCxStatus::Ok) {
    return status;
  }
  status = readRegister16(measurementLsbRegister(measurement), lsb);
  if (status != FDCxStatus::Ok) {
    return status;
  }

  const uint32_t rawUnsigned = (static_cast<uint32_t>(msb) << 8) |
                               ((static_cast<uint32_t>(lsb) >> 8) & 0xFFu);
  raw24 = signExtend24(rawUnsigned);
  return setStatus(FDCxStatus::Ok);
}

FDCxStatus FDC1004::readCapacitancePf(FDC1004Measurement measurement, float& capacitancePf) {
  // Compatibility name: this reads the latest completed measurement registers.
  // Use readSingleMeasurement() when a fresh single-shot trigger/wait/read flow is required.
  capacitancePf = 0.0f;
  if (!isValidMeasurement(measurement)) {
    return setStatus(FDCxStatus::InvalidArgument);
  }

  const uint8_t idx = measurementIndex(measurement);
  if (!_measurementConfigured[idx]) {
    return setStatus(FDCxStatus::MeasurementNotReady);
  }

  int32_t raw = 0;
  const FDCxStatus status = readRaw24(measurement, raw);
  if (status != FDCxStatus::Ok) {
    return status;
  }

  capacitancePf = (static_cast<float>(raw) / fdc1004_registers::kRawDenominator) + capdacOffsetPf(measurement);
  return setStatus(FDCxStatus::Ok);
}

FDCxStatus FDC1004::readLastCapacitancePf(FDC1004Measurement measurement, float& capacitancePf) {
  return readCapacitancePf(measurement, capacitancePf);
}

FDCxStatus FDC1004::readCapacitanceReading(FDC1004Measurement measurement, FDC1004Reading& reading) {
  reading = FDC1004Reading();
  if (!isValidMeasurement(measurement)) {
    reading.status = FDCxStatus::InvalidArgument;
    return setStatus(FDCxStatus::InvalidArgument);
  }
  const uint8_t idx = measurementIndex(measurement);
  if (!_measurementConfigured[idx]) {
    reading.status = FDCxStatus::MeasurementNotReady;
    return setStatus(FDCxStatus::MeasurementNotReady);
  }
  int32_t raw = 0;
  float pf = 0.0f;
  const FDCxStatus status = readSingleMeasurement(measurement, raw, pf);
  reading.channel = static_cast<uint8_t>(_measurementChannels[idx]);
  reading.raw24 = raw;
  reading.capacitancePf = pf;
  reading.capdacCode = _capdacCodes[idx];
  reading.capdacEnabled = _capdacEnabled[idx];
  reading.status = status;
  reading.diagnosticFlags = 0;
  return status;
}

FDCxStatus FDC1004::readSingleMeasurement(FDC1004Measurement measurement, int32_t& raw24, float& capacitancePf) {
  raw24 = 0;
  capacitancePf = 0.0f;

  const uint8_t idx = measurementIndex(measurement);
  if (!_measurementConfigured[idx]) {
    return setStatus(FDCxStatus::MeasurementNotReady);
  }

  FDCxStatus status = triggerSingleMeasurement(measurement);
  if (status != FDCxStatus::Ok) {
    return status;
  }
  status = waitForMeasurement(measurement, _timeoutMs);
  if (status != FDCxStatus::Ok) {
    return status;
  }
  status = readRaw24(measurement, raw24);
  if (status != FDCxStatus::Ok) {
    return status;
  }

  capacitancePf = (static_cast<float>(raw24) / fdc1004_registers::kRawDenominator) + capdacOffsetPf(measurement);
  return setStatus(FDCxStatus::Ok);
}

uint16_t FDC1004::buildSingleEndedConfig(FDC1004Channel channel,
                                         FDC1004CapdacMode mode,
                                         uint8_t capdacCode,
                                         FDCxStatus* status) const {
  if (!isValidChannel(channel)) {
    if (status != nullptr) {
      *status = FDCxStatus::InvalidArgument;
    }
    return 0;
  }
  if (mode == FDC1004CapdacMode::Disabled && capdacCode != 0) {
    if (status != nullptr) {
      *status = FDCxStatus::InvalidArgument;
    }
    return 0;
  }
  if (mode == FDC1004CapdacMode::Enabled && capdacCode > fdc1004_registers::kMaxCapdacCode) {
    if (status != nullptr) {
      *status = FDCxStatus::InvalidArgument;
    }
    return 0;
  }

  if (status != nullptr) {
    *status = FDCxStatus::Ok;
  }

  const uint16_t cha = static_cast<uint16_t>(channel) & 0x07u;
  const uint16_t chb = (mode == FDC1004CapdacMode::Enabled)
                           ? (fdc1004_registers::kChbCapdac & 0x07u)
                           : (fdc1004_registers::kChbDisabled & 0x07u);
  const uint16_t capdac = (mode == FDC1004CapdacMode::Enabled)
                              ? (static_cast<uint16_t>(capdacCode) & 0x1Fu)
                              : 0u;

  return static_cast<uint16_t>((cha << 13) | (chb << 10) | (capdac << 5));
}

uint16_t FDC1004::buildSingleEndedConfig(FDC1004Channel channel, uint8_t capdacCode, FDCxStatus* status) const {
  return buildSingleEndedConfig(channel, FDC1004CapdacMode::Enabled, capdacCode, status);
}

uint16_t FDC1004::currentConfigRegisterValue() const {
  return _fdcConfig;
}

uint8_t FDC1004::address() const {
  return _address;
}

bool FDC1004::capdacEnabled(FDC1004Measurement measurement) const {
  return isValidMeasurement(measurement) ? _capdacEnabled[measurementIndex(measurement)] : false;
}

uint8_t FDC1004::capdacCode(FDC1004Measurement measurement) const {
  return isValidMeasurement(measurement) ? _capdacCodes[measurementIndex(measurement)] : 0;
}

bool FDC1004::isValidMeasurement(FDC1004Measurement measurement) {
  const uint8_t index = static_cast<uint8_t>(measurement);
  return index <= 3u;
}

bool FDC1004::isValidChannel(FDC1004Channel channel) {
  const uint8_t index = static_cast<uint8_t>(channel);
  return index <= 3u;
}

uint8_t FDC1004::measurementIndex(FDC1004Measurement measurement) {
  return static_cast<uint8_t>(measurement);
}

uint8_t FDC1004::measurementMsbRegister(FDC1004Measurement measurement) {
  return static_cast<uint8_t>(fdc1004_registers::kMeasurement1Msb + (measurementIndex(measurement) * 2u));
}

uint8_t FDC1004::measurementLsbRegister(FDC1004Measurement measurement) {
  return static_cast<uint8_t>(fdc1004_registers::kMeasurement1Lsb + (measurementIndex(measurement) * 2u));
}

uint8_t FDC1004::measurementConfigRegister(FDC1004Measurement measurement) {
  return static_cast<uint8_t>(fdc1004_registers::kMeasurementConfig1 + measurementIndex(measurement));
}

uint16_t FDC1004::measurementTriggerBit(FDC1004Measurement measurement) {
  return static_cast<uint16_t>(1u << (7u - measurementIndex(measurement)));
}

uint16_t FDC1004::measurementDoneBit(FDC1004Measurement measurement) {
  return static_cast<uint16_t>(1u << (3u - measurementIndex(measurement)));
}

int32_t FDC1004::signExtend24(uint32_t raw24Unsigned) {
  raw24Unsigned &= 0x00FFFFFFu;
  if ((raw24Unsigned & 0x00800000u) != 0u) {
    raw24Unsigned |= 0xFF000000u;
  }
  return static_cast<int32_t>(raw24Unsigned);
}

float FDC1004::capdacOffsetPf(FDC1004Measurement measurement) const {
  if (!isValidMeasurement(measurement)) {
    return 0.0f;
  }
  const uint8_t index = measurementIndex(measurement);
  if (!_capdacEnabled[index]) {
    return 0.0f;
  }
  return static_cast<float>(_capdacCodes[index]) * fdc1004_registers::kCapdacStepPf;
}

FDCxStatus FDC1004::setStatus(FDCxStatus status) {
  _lastStatus = status;
  return status;
}

FDCxStatus FDC1004::readIdentification() {
  uint16_t manufacturer = 0;
  FDCxStatus status = readRegister16(fdc1004_registers::kManufacturerId, manufacturer);
  if (status != FDCxStatus::Ok) {
    return setStatus(FDCxStatus::DeviceNotFound);
  }
  if (manufacturer != fdc1004_registers::kExpectedManufacturerId) {
    return setStatus(FDCxStatus::InvalidDeviceId);
  }

  uint16_t device = 0;
  status = readRegister16(fdc1004_registers::kDeviceId, device);
  if (status != FDCxStatus::Ok) {
    return setStatus(FDCxStatus::DeviceNotFound);
  }
  if (device != fdc1004_registers::kExpectedDeviceId) {
    return setStatus(FDCxStatus::InvalidDeviceId);
  }

  return setStatus(FDCxStatus::Ok);
}
