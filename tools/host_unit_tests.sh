#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
TMPDIR=${TMPDIR:-/tmp}
WORK=$(mktemp -d "$TMPDIR/fdcx-host-unit.XXXXXX")
cleanup() { rm -rf "$WORK"; }
trap cleanup EXIT
mkdir -p "$WORK/include"
cat > "$WORK/include/Arduino.h" <<'STUB'
#pragma once
#include <stdint.h>
#include <stddef.h>
#define HIGH 0x1
#define LOW 0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
inline unsigned long millis() { static unsigned long t = 0; return t += 10; }
inline void delay(unsigned long) {}
class Print {
 public:
  template <typename T> void print(const T&) {}
  template <typename T> void println(const T&) {}
  void println() {}
  void println(float, int) {}
  void print(float, int) {}
};
class HardwareSerial : public Print {
 public:
  void begin(unsigned long) {}
  operator bool() const { return true; }
};
extern HardwareSerial Serial;
STUB
cat > "$WORK/include/Wire.h" <<'STUB'
#pragma once
#include <stdint.h>
#include <stddef.h>
class TwoWire {
 public:
  void begin() {}
  void beginTransmission(uint8_t) {}
  uint8_t endTransmission(bool stop = true) { (void)stop; return 0; }
  size_t requestFrom(uint8_t, uint8_t) { return 0; }
  size_t requestFrom(uint8_t, size_t) { return 0; }
  size_t write(uint8_t) { return 1; }
  int available() { return 0; }
  int read() { return 0; }
};
extern TwoWire Wire;
STUB
cat > "$WORK/stubs.cpp" <<'STUB'
#include <Arduino.h>
#include <Wire.h>
HardwareSerial Serial;
TwoWire Wire;
STUB
cat > "$WORK/host_unit_tests.cpp" <<'CPP'
#include <FDCxCapSense.h>
#include <core/FDCxSerialization.h>
#include "FakeI2C.h"
#include <cassert>
#include <cmath>
#include <limits>

static void assertStatus(FDCxStatus expected, FDCxStatus actual) {
  assert(static_cast<int>(expected) == static_cast<int>(actual));
}

static bool sawConfigWriteState(const FakeI2C& bus, uint8_t channel, bool sleepEnabled) {
  for (uint8_t i = 0; i < bus.writeLogCount; ++i) {
    if (bus.writeLogReg[i] != fdc2x1x_registers::kConfig) continue;
    const uint16_t value = bus.writeLogValue[i];
    const bool sleep = (value & fdc2x1x_registers::kConfigSleepModeMask) != 0u;
    if (sleep == sleepEnabled && fdc2x1x_registers::activeChannelFromConfig(value) == channel &&
        fdc2x1x_registers::configReservedPatternIsValid(value)) {
      return true;
    }
  }
  return false;
}

static void testFdc2StartupSleepAndActiveSwitch() {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertStatus(FDCxStatus::Ok, fdc.begin(bus, FDC2x1xVariant::FDC2214));
  assert(fdc.isSleepModeEnabled());
  assertStatus(FDCxStatus::Ok, fdc.setActiveChannel(FDC2x1xChannel::CH0));
  assertStatus(FDCxStatus::Ok, fdc.startContinuous(FDC2x1xChannel::CH0));
  assert(!fdc.isSleepModeEnabled());
  assertStatus(FDCxStatus::InvalidArgument, fdc.setActiveChannel(FDC2x1xChannel::CH1));
  const uint8_t beforeSwitchLogCount = bus.writeLogCount;
  (void)beforeSwitchLogCount;
  assertStatus(FDCxStatus::Ok, fdc.startContinuous(FDC2x1xChannel::CH1));
  const uint16_t config = bus.getRegister(fdc2x1x_registers::kConfig);
  assert((config & fdc2x1x_registers::kConfigSleepModeMask) == 0u);
  assert(fdc2x1x_registers::activeChannelFromConfig(config) == 1u);
  assert(sawConfigWriteState(bus, 0u, true));
  assert(sawConfigWriteState(bus, 1u, true));
  assert(sawConfigWriteState(bus, 1u, false));
  assertStatus(FDCxStatus::Ok, fdc.startContinuous(FDC2x1xChannel::CH1));
  assertStatus(FDCxStatus::InvalidArgument, fdc.startContinuous(static_cast<FDC2x1xChannel>(4)));
}

static void testFdc2ConfigMuxErrorAndResetValidation() {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2212);
  FDC2x1x fdc;
  assertStatus(FDCxStatus::Ok, fdc.begin(bus, FDC2x1xVariant::FDC2212));
  assertStatus(FDCxStatus::InvalidArgument,
               fdc.writeRegister16(fdc2x1x_registers::kConfig,
                                    fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH2, false)));
  assertStatus(FDCxStatus::Ok, fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0x820Du));
  assertStatus(FDCxStatus::Ok, fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0xE20Du));
  assertStatus(FDCxStatus::InvalidArgument, fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0xA20Du));
  assertStatus(FDCxStatus::InvalidArgument, fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0xC20Du));
  assertStatus(FDCxStatus::Ok, fdc.writeRegister16(fdc2x1x_registers::kErrorConfig,
                                                   fdc2x1x_registers::kErrorConfigAllowedMask));
  assertStatus(FDCxStatus::InvalidArgument, fdc.writeRegister16(fdc2x1x_registers::kErrorConfig, 0xFFFFu));
  assertStatus(FDCxStatus::InvalidArgument,
               fdc.writeRegister16(fdc2x1x_registers::kResetDev, fdc2x1x_registers::kResetDevOutputGainMask));
  assertStatus(FDCxStatus::Ok,
               fdc.writeRegister16(fdc2x1x_registers::kResetDev, fdc2x1x_registers::kResetDevResetMask));
}

static void testFdc2ClockDividerValidation() {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertStatus(FDCxStatus::Ok, fdc.begin(bus, FDC2x1xVariant::FDC2214));
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800u);
  bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000u);
  FDC2x1xFrequency frequency;
  bus.setRegister(fdc2x1x_registers::clockDividersRegister(0), 0u);
  assertStatus(FDCxStatus::InvalidArgument, fdc.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, frequency));
  assertStatus(FDCxStatus::Ok, fdc.writeRegister16(fdc2x1x_registers::kMuxConfig,
                                                   fdc2x1x_registers::singleChannelMuxValue(FDC2x1xDeglitch::MHz_10)));
  assertStatus(FDCxStatus::Ok, fdc.configureClockDividersForSingleEnded(FDC2x1xChannel::CH0, 1u));
  assertStatus(FDCxStatus::Ok, fdc.startContinuous(FDC2x1xChannel::CH0));
  assertStatus(FDCxStatus::Ok, fdc.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, frequency));
  assert(frequency.finSelMultiplier == 2u);
  assert(frequency.frefDivider == 1u);
  assert(frequency.frequencyHz > 0.0f);
}


static FDCxFeatureVector hostDirectFeatures(FDCxStatus status = FDCxStatus::Ok) {
  FDCxSample sample = FDCxSample::directFDC1004(0, 3.0f, status, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.02f, 0.0f, 0, true);
  features.temperatureC = 25.0f;
  features.hasTemperature = true;
  return features;
}

static void testFdc2FinalPolishNoSideEffectsAndMetadata() {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertStatus(FDCxStatus::Ok, fdc.begin(bus, FDC2x1xVariant::FDC2214));
  assertStatus(FDCxStatus::Ok, fdc.writeRegister16(fdc2x1x_registers::kConfig,
      fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH0, false,
                                         fdc2x1x_registers::kConfigHighCurrentMask)));
  const uint16_t configBefore = bus.getRegister(fdc2x1x_registers::kConfig);
  const uint8_t logBefore = bus.writeLogCount;
  assertStatus(FDCxStatus::InvalidArgument, fdc.startContinuous(FDC2x1xChannel::CH1));
  assert(bus.getRegister(fdc2x1x_registers::kConfig) == configBefore);
  assert(bus.writeLogCount == logBefore);
  assertStatus(FDCxStatus::Ok, fdc.startContinuous(FDC2x1xChannel::CH0));

  assertStatus(FDCxStatus::Ok, fdc.stopConversions());
  assertStatus(FDCxStatus::Ok, fdc.setClockDividers(
      FDC2x1xChannel::CH0,
      static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(2) | 3u)));
  assertStatus(FDCxStatus::Ok, fdc.startContinuous(FDC2x1xChannel::CH0));
  assertStatus(FDCxStatus::InvalidArgument, fdc.setClockDividers(
      FDC2x1xChannel::CH0,
      static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(2) | 4u)));
  assertStatus(FDCxStatus::Ok, fdc.writeRawClockDividers(
      FDC2x1xChannel::CH0,
      static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(2) | 3u)));

  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800u);
  bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000u);
  FDC2x1xFrequency frequency;
  assertStatus(FDCxStatus::Ok, fdc.readFrequencyHz(FDC2x1xChannel::CH0, 20000000.0f, frequency));
  assert(frequency.finSelMultiplier == 2u);
  assert(frequency.frefDivider == 3u);
}

static void testFdc2ConfigureSingleChannelPrevalidation() {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertStatus(FDCxStatus::Ok, fdc.begin(bus, FDC2x1xVariant::FDC2214));
  const uint8_t logBefore = bus.writeLogCount;
  assertStatus(FDCxStatus::InvalidArgument,
               fdc.configureSingleChannel(FDC2x1xChannel::CH0, 0x00FFu, 0x0100u, 2u, 1u, 10u,
                                          FDC2x1xDeglitch::MHz_10));
  assert(bus.writeLogCount == logBefore);
}

static void testAutomotiveTimestampWraparound() {
  FDCxAutomotiveProximityConfig cfg;
  cfg.debounceSamples = 1;
  cfg.touchlessCandidateThresholdPf = 2.0f;
  cfg.handPresenceThresholdPf = 1.0f;
  FDCxAutomotiveProximityProfile profile(cfg);
  FDCxFeatureVector input = hostDirectFeatures();
  input.compensatedDeltaPf = 0.25f;
  assertStatus(FDCxStatus::Ok, profile.update(input, 0xFFFFFFF0UL).status);
  assertStatus(FDCxStatus::Ok, profile.update(input, 20UL).status);

  FDCxAutomotiveProximityProfile backwards(cfg);
  assertStatus(FDCxStatus::Ok, backwards.update(input, 1000UL).status);
  assertStatus(FDCxStatus::InvalidArgument, backwards.update(input, 900UL).status);
}

static void testSharedMathAndSerializationHelpers() {
  assert(fdcxClamp01Finite(0.5f) == 0.5f);
  assert(fdcxClamp01Finite(-1.0f) == 0.0f);
  assert(fdcxClamp01Finite(2.0f) == 1.0f);
  const float nanValue = std::numeric_limits<float>::quiet_NaN();
  assert(fdcxClamp01Finite(nanValue) == 0.0f);
  assert(!fdcxIsFiniteFloat(std::numeric_limits<float>::infinity()));

  uint8_t buffer[12] = {};
  fdcxPutU16(buffer, 0x1234u);
  fdcxPutU32(buffer + 2, 0x89ABCDEFUL);
  fdcxPutFloat(buffer + 6, 1.25f);
  assert(fdcxGetU16(buffer) == 0x1234u);
  assert(fdcxGetU32(buffer + 2) == 0x89ABCDEFUL);
  assert(fdcxGetFloat(buffer + 6) > 1.24f && fdcxGetFloat(buffer + 6) < 1.26f);
}

static void testFdc1004LatestVsFresh() {
  FakeI2C bus;
  FDC1004 fdc;
  assertStatus(FDCxStatus::Ok, fdc.begin(bus));
  float pf = 0.0f;
  int32_t raw = 0;
  assertStatus(FDCxStatus::MeasurementNotReady, fdc.readLastCapacitancePf(FDC1004Measurement::M1, pf));
  assertStatus(FDCxStatus::Ok, fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN1));
  bus.setSimulatedCapacitance(0, 1.5f);
  assertStatus(FDCxStatus::Ok, fdc.readSingleMeasurement(FDC1004Measurement::M1, raw, pf));
  assert(pf > 1.4f && pf < 1.6f);
  const uint16_t writesAfterFresh = bus.writeHits[fdc1004_registers::kFdcConfig];
  assertStatus(FDCxStatus::Ok, fdc.readLastCapacitancePf(FDC1004Measurement::M1, pf));
  assert(bus.writeHits[fdc1004_registers::kFdcConfig] == writesAfterFresh);
}

int main() {
  testFdc2StartupSleepAndActiveSwitch();
  testFdc2ConfigMuxErrorAndResetValidation();
  testFdc2ClockDividerValidation();
  testFdc2FinalPolishNoSideEffectsAndMetadata();
  testFdc2ConfigureSingleChannelPrevalidation();
  testAutomotiveTimestampWraparound();
  testSharedMathAndSerializationHelpers();
  testFdc1004LatestVsFresh();
  return 0;
}
CPP
CXX=${CXX:-g++}
COMMON="-std=c++11 -Wall -Wextra -Werror -I$WORK/include -Isrc -Iextras/tests/fake_i2c"
SOURCES="src/fdcx/FDC1004.cpp src/fdcx/FDC1004AutoCapdac.cpp src/fdcx/FDC1004Calibration.cpp src/fdcx/FDCxBaseline.cpp src/fdcx/FDCxTemperatureCompensation.cpp src/fdcx/FDCxDiagnostics.cpp src/fdcx/FDCxProfile.cpp src/fdcx/profiles/FDCxProfileTypes.cpp src/fdcx/profiles/FDCxDebounce.cpp src/fdcx/profiles/FDCxLiquidLevelProfile.cpp src/fdcx/profiles/FDCxProximityProfile.cpp src/fdcx/profiles/FDCxSurfaceStateProfile.cpp src/fdcx/profiles/FDCxMaterialProfile.cpp src/fdcx/profiles/FDCxPowderGranularProfile.cpp src/fdcx/profiles/FDCxAutomotiveProximityProfile.cpp src/devices/fdc2x1x/FDC2x1x.cpp src/devices/fdc2x1x/FDC2x1xConversion.cpp src/calibration/fdc2x1x/FDC2x1xCalibrationProfile.cpp extras/tests/fake_i2c/FakeI2C.cpp"
$CXX $COMMON "$WORK/stubs.cpp" "$WORK/host_unit_tests.cpp" $SOURCES -o "$WORK/host_unit_tests"
"$WORK/host_unit_tests"
echo "PASS: host unit tests executable passed"
