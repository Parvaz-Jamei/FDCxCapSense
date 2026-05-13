#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
TMPDIR=${TMPDIR:-/tmp}
WORK=$(mktemp -d "$TMPDIR/fdcx-host-smoke.XXXXXX")
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
cat > "$WORK/include/AUnit.h" <<'STUB'
#pragma once
#include <stdint.h>
#define test(name) void test_##name()
#define assertEqual(a,b) do { (void)(a); (void)(b); } while(0)
#define assertTrue(a) do { (void)(a); } while(0)
#define assertFalse(a) do { (void)(a); } while(0)
#define assertNear(a,b,c) do { (void)(a); (void)(b); (void)(c); } while(0)
#define assertMoreOrEqual(a,b) do { (void)(a); (void)(b); } while(0)
#define assertLessOrEqual(a,b) do { (void)(a); (void)(b); } while(0)
namespace aunit { struct TestRunner { static void run() {} }; }
STUB
cat > "$WORK/stubs.cpp" <<'STUB'
#include <Arduino.h>
#include <Wire.h>
HardwareSerial Serial;
TwoWire Wire;
STUB
CXX=${CXX:-g++}
COMMON="-std=c++11 -Wall -Wextra -Werror -I$WORK/include -Isrc -Iextras/tests/fake_i2c"
SOURCES="src/fdcx/FDC1004.cpp src/fdcx/FDC1004AutoCapdac.cpp src/fdcx/FDC1004Calibration.cpp src/fdcx/FDCxBaseline.cpp src/fdcx/FDCxTemperatureCompensation.cpp src/fdcx/FDCxDiagnostics.cpp src/fdcx/FDCxProfile.cpp src/fdcx/profiles/FDCxProfileTypes.cpp src/fdcx/profiles/FDCxDebounce.cpp src/fdcx/profiles/FDCxLiquidLevelProfile.cpp src/fdcx/profiles/FDCxProximityProfile.cpp src/fdcx/profiles/FDCxSurfaceStateProfile.cpp src/fdcx/profiles/FDCxMaterialProfile.cpp src/fdcx/profiles/FDCxPowderGranularProfile.cpp src/fdcx/profiles/FDCxAutomotiveProximityProfile.cpp src/devices/fdc2x1x/FDC2x1x.cpp src/devices/fdc2x1x/FDC2x1xConversion.cpp src/calibration/fdc2x1x/FDC2x1xCalibrationProfile.cpp extras/tests/fake_i2c/FakeI2C.cpp"
$CXX $COMMON -fsyntax-only $SOURCES
$CXX $COMMON -DFDCX_DISABLE_LEGACY_PROFILE_HELPERS -fsyntax-only $SOURCES
for ino in $(find examples extras/tests -name "*.ino" | sort); do
  $CXX $COMMON -x c++ -fsyntax-only "$ino"
done
cat > "$WORK/smoke.cpp" <<'STUB'
#include <FDCxCapSense.h>
#include "FakeI2C.h"
#include <cassert>
#include <limits>

static FDCxFeatureVector directFeatures(FDCxStatus status = FDCxStatus::Ok) {
  FDC1004Reading reading;
  reading.channel = 0;
  reading.capacitancePf = 3.0f;
  reading.status = status;
  FDCxSample sample = reading.toSample();
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 2.0f, 0.02f, 0.0f, 0, true);
  features.temperatureC = -1.0f;
  features.hasTemperature = true;
  return features;
}

static FDCxFeatureVector derivedFeatures() {
  FDCxSample derived;
  derived.family = FDCxFamily::FDC2x1x_ResonantLC;
  derived.model = FDCxMeasurementModel::DerivedCapacitance;
  derived.hasCapacitance = true;
  derived.capacitanceIsDirect = false;
  derived.capacitanceIsDerived = true;
  derived.capacitancePf = 3.0f;
  derived.status = FDCxStatus::Ok;
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(derived, 2.0f, 0.02f, 0.0f, 0, true);
  features.temperatureC = -1.0f;
  features.hasTemperature = true;
  return features;
}

int main() {
  FDCxFeatureVector ok = directFeatures();
  assert(fdcxProfileInputIsDirectFDC1004Capacitance(ok));
  FDCxSample badFdc1004 = FDCxSample::directFDC1004(0, 5.0f, FDCxStatus::Timeout, 0, 1.0f);
  assert(!badFdc1004.hasCapacitance);
  assert(!badFdc1004.capacitanceIsDirect);
  assert(badFdc1004.capacitancePf == 0.0f);

  FDCxSurfaceStateConfig surfaceCfg;
  surfaceCfg.minConfidence = 0.0f;
  FDCxSurfaceStateProfile surface(surfaceCfg);
  assert(surface.update(ok).status == FDCxStatus::Ok);
  assert(surface.update(derivedFeatures()).status == FDCxStatus::InvalidArgument);
  assert(surface.update(directFeatures(FDCxStatus::Timeout)).status == FDCxStatus::Timeout);

  FDCxLiquidLevelConfig liquidCfg;
  FDCxLiquidLevelProfile liquid(liquidCfg);
  assert(liquid.update(derivedFeatures()).status == FDCxStatus::InvalidArgument);

  FDCxProximityConfig proxCfg;
  proxCfg.debounceSamples = 1;
  FDCxProximityProfile proximity(proxCfg);
  assert(proximity.update(derivedFeatures()).status == FDCxStatus::InvalidArgument);
  assert(proximity.update(directFeatures(FDCxStatus::I2cError)).status == FDCxStatus::I2cError);

  FDCxGranularConfig granularCfg;
  FDCxPowderGranularProfile granular(granularCfg);
  assert(granular.update(derivedFeatures()).status == FDCxStatus::InvalidArgument);
  assert(granular.update(directFeatures(FDCxStatus::MeasurementNotReady)).status == FDCxStatus::MeasurementNotReady);

  FDCxAutomotiveProximityConfig autoCfg;
  FDCxAutomotiveProximityProfile automotive(autoCfg);
  assert(automotive.update(derivedFeatures(), 10).status == FDCxStatus::InvalidArgument);

  FDCxMaterialSignature signature;
  assert(computeMaterialSignature(derivedFeatures(), signature) == FDCxStatus::InvalidArgument);
  assert(computeMaterialSignature(directFeatures(FDCxStatus::Timeout), signature) == FDCxStatus::Timeout);

  FDC1004CalibrationProfile profile;
  profile.capdacCode[0] = 3;
  profile.baselinePf[0] = 1.25f;
  uint8_t buffer[kFDC1004CalibrationProfileSerializedSize] = {};
  assert(fdcxSerializeProfile(profile, buffer, sizeof(buffer)));
  FDC1004CalibrationProfile parsed;
  assert(fdcxDeserializeProfile(buffer, sizeof(buffer), parsed));
  assert(parsed.family == FDCxFamily::FDC1004_DirectCDC);
  assert(parsed.chip == FDCxChipId::FDC1004);
  buffer[0] ^= 0x01u;
  assert(!fdcxDeserializeProfile(buffer, sizeof(buffer), parsed));
  profile.capdacCode[0] = 32;
  assert(!fdcxSerializeProfile(profile, buffer, sizeof(buffer)));

  FDCxFeatureVector noisy = directFeatures();
  noisy.noisePf = -0.1f;
  assert(surface.update(noisy).status == FDCxStatus::InvalidArgument);

  FDCxLiquidLevelConfig refLiquidCfg;
  refLiquidCfg.useReferenceChannel = true;
  refLiquidCfg.referenceBaselineValid = true;
  FDCxLiquidLevelProfile refLiquid(refLiquidCfg);
  assert(refLiquid.update(ok).status == FDCxStatus::MeasurementNotReady);

  FDC2x1xFrequency frequency;
  frequency.status = FDCxStatus::Ok;
  frequency.frequencyHz = 1000000.0f;
  FDC2x1xLCParams params;
  params.inductanceUH = 18.0f;
  params.parasiticCapacitancePf = 2.0f;
  params.traceReferenceClockHz = 40000000.0f;
  FDC2x1xDerivedCapacitance derivedCap;
  assert(fdc2x1xDeriveCapacitancePf(frequency, params, derivedCap) == FDCxStatus::Ok);
  assert(derivedCap.toSample().family == FDCxFamily::FDC2x1x_ResonantLC);
  assert(derivedCap.toSample().capacitanceIsDerived);
  assert(derivedCap.toSample(std::numeric_limits<float>::quiet_NaN()).confidence == 0.0f);
  assert(derivedCap.toSample(-0.5f).confidence == 0.0f);
  assert(derivedCap.toSample(2.0f).confidence == 1.0f);
  assert(surface.update(FDCxFeatureVector::fromSample(derivedCap.toSample(), 2.0f, 0.02f, 0.0f, 0, true)).status == FDCxStatus::InvalidArgument);

  FakeI2C hardeningBus;
  hardeningBus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x hardening;
  assert(hardening.begin(hardeningBus, FDC2x1xVariant::FDC2214) == FDCxStatus::Ok);
  assert(hardening.writeRegister16(fdc2x1x_registers::kErrorConfig, fdc2x1x_registers::kErrorConfigAllowedMask) == FDCxStatus::Ok);
  assert(hardening.writeRegister16(fdc2x1x_registers::kErrorConfig, 0xFFFFu) == FDCxStatus::InvalidArgument);
  assert(hardening.setRCount(FDC2x1xChannel::CH0, 0x0100u) == FDCxStatus::Ok);
  assert(hardening.setSettleCount(FDC2x1xChannel::CH0, 0x0100u) == FDCxStatus::Ok);
  assert(hardening.configureClockDividersForSingleEnded(FDC2x1xChannel::CH0, 2u) == FDCxStatus::Ok);
  assert(hardening.setDriveCurrentCode(FDC2x1xChannel::CH0, 10u) == FDCxStatus::Ok);
  assert(hardening.setDeglitch(FDC2x1xDeglitch::MHz_10) == FDCxStatus::Ok);
  assert(hardening.setReferenceClockSource(FDC2x1xReferenceClockSource::ExternalClockInput) == FDCxStatus::Ok);
  assert(hardening.startContinuous(FDC2x1xChannel::CH0) == FDCxStatus::Ok);
  assert(!hardening.isSleepModeEnabled());
  assert(hardening.setRCount(FDC2x1xChannel::CH0, 0x0100u) == FDCxStatus::InvalidArgument);
  assert(hardening.setSettleCount(FDC2x1xChannel::CH0, 0x0100u) == FDCxStatus::InvalidArgument);
  assert(hardening.configureClockDividersForSingleEnded(FDC2x1xChannel::CH0, 2u) == FDCxStatus::InvalidArgument);
  assert(hardening.setDriveCurrentCode(FDC2x1xChannel::CH0, 10u) == FDCxStatus::InvalidArgument);
  assert(hardening.setDeglitch(FDC2x1xDeglitch::MHz_10) == FDCxStatus::InvalidArgument);
  assert(hardening.setReferenceClockSource(FDC2x1xReferenceClockSource::InternalOscillator) == FDCxStatus::InvalidArgument);
  assert(hardening.stopConversions() == FDCxStatus::Ok);
  assert(hardening.isSleepModeEnabled());
  assert(hardening.configureClockDividersForDifferential(FDC2x1xChannel::CH0, 1u, 2u) == FDCxStatus::Ok);
  assert(hardening.configureClockDividersForDifferential(FDC2x1xChannel::CH0, 2u, 2u) == FDCxStatus::Ok);
  assert(hardening.configureClockDividersForDifferential(FDC2x1xChannel::CH0, 3u, 2u) == FDCxStatus::InvalidArgument);
  hardeningBus.setRegister(fdc2x1x_registers::clockDividersRegister(0),
                           static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(2) | 2u));
  hardeningBus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800);
  hardeningBus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000);
  assert(hardening.startContinuous(FDC2x1xChannel::CH0) == FDCxStatus::Ok);
  FDC2x1xFrequency legacySafe;
  assert(hardening.readFrequencyHz(FDC2x1xChannel::CH0, 20000000.0f, legacySafe) == FDCxStatus::Ok);
  assert(legacySafe.finSelMultiplier == 2);


  FakeI2C fdc2bus;
  fdc2bus.expectedAddress = fdc2x1x_registers::kDefaultI2cAddressLow;
  fdc2bus.setRegister(fdc2x1x_registers::kManufacturerId, fdc2x1x_registers::kExpectedManufacturerId);
  fdc2bus.setRegister(fdc2x1x_registers::kDeviceId, fdc2x1x_registers::kExpectedDeviceIdFdc221x);
  fdc2bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800);
  fdc2bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000);
  fdc2bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh0UnreadMask);
  fdc2bus.setRegister(fdc2x1x_registers::clockDividersRegister(0),
                      static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(2) | 2u));
  FDC2x1x fdc2;
  assert(fdc2.begin(fdc2bus, FDC2x1xVariant::FDC2214) == FDCxStatus::Ok);
  assert(fdc2.waitDataReady(FDC2x1xChannel::CH0, 10) == FDCxStatus::Ok);
  FDC2x1xFrequency readFrequency;
  assert(fdc2.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, readFrequency) == FDCxStatus::Ok);
  assert(readFrequency.finSelMultiplier == 2);
  assert(readFrequency.frefDivider == 2);
  assert(readFrequency.toSample().model == FDCxMeasurementModel::ResonantFrequency);
  fdc2bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh1UnreadMask);
  assert(fdc2.waitDataReady(FDC2x1xChannel::CH0, 1) == FDCxStatus::Timeout);
  fdc2bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusErrorMask | fdc2x1x_registers::kStatusCh0UnreadMask);
  assert(fdc2.waitDataReady(FDC2x1xChannel::CH0, 10) == FDCxStatus::MeasurementNotReady);
  fdc2bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), fdc2x1x_registers::kDataErrWdMask);
  uint32_t badRaw = 1;
  assert(fdc2.readRaw(FDC2x1xChannel::CH0, badRaw) == FDCxStatus::MeasurementNotReady);
  assert(badRaw == 0);
  assert(fdc2.writeRegister16(fdc2x1x_registers::kStatus, 0) == FDCxStatus::InvalidArgument);
  assert(fdc2.idle() == FDCxStatus::Ok);

  FakeI2C fdc2112bus;
  fdc2112bus.expectedAddress = fdc2x1x_registers::kDefaultI2cAddressLow;
  fdc2112bus.setRegister(fdc2x1x_registers::kManufacturerId, fdc2x1x_registers::kExpectedManufacturerId);
  fdc2112bus.setRegister(fdc2x1x_registers::kDeviceId, fdc2x1x_registers::kExpectedDeviceIdFdc211x);
  fdc2112bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0A55);
  FDC2x1x fdc2112;
  assert(fdc2112.begin(fdc2112bus, FDC2x1xVariant::FDC2112) == FDCxStatus::Ok);
  uint32_t raw2112 = 0;
  assert(fdc2112.readRaw(FDC2x1xChannel::CH0, raw2112) == FDCxStatus::Ok);
  assert(raw2112 == 0x0A55UL);
  assert(fdc2112bus.readHits[fdc2x1x_registers::dataLsbRegister(0)] == 0);
  assert(fdc2112.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, readFrequency) == FDCxStatus::NotSupported);
  assert(fdc2112.configureChannel(FDC2x1xChannel::CH2) == FDCxStatus::InvalidArgument);

  FDC2x1xCalibrationProfile fdc2profile;
  fdc2profile.variant = FDC2x1xVariant::FDC2214;
  fdc2profile.chip = FDCxChipId::FDC2214;
  fdc2profile.referenceClockHz = 40000000.0f;
  for (uint8_t i = 0; i < 4; ++i) {
    fdc2profile.inductanceUH[i] = 18.0f;
    fdc2profile.parasiticCapacitancePf[i] = 2.0f;
    fdc2profile.baselineFrequencyHz[i] = 1000000.0f;
  }
  uint8_t fdc2ProfileBuffer[kFDC2x1xCalibrationProfileSerializedSize] = {};
  assert(fdc2x1xSerializeProfile(fdc2profile, fdc2ProfileBuffer, sizeof(fdc2ProfileBuffer)));
  FDC2x1xCalibrationProfile parsedFdc2Profile;
  assert(fdc2x1xDeserializeProfile(fdc2ProfileBuffer, sizeof(fdc2ProfileBuffer), parsedFdc2Profile));
  fdc2ProfileBuffer[0] ^= 0x01u;
  assert(!fdc2x1xDeserializeProfile(fdc2ProfileBuffer, sizeof(fdc2ProfileBuffer), parsedFdc2Profile));

  FDC1004 device;
  FDCxCapability caps = device.capability();
  assert(caps.supportsDirectCapacitance);
  assert(!caps.supportsResonantFrequency);
  assert(!caps.chipUsesResonantFrequency);
  assert(!caps.driverSupportsFrequencyConversion);
  assert(!caps.supportsHardwareInterrupt);
  return 0;
}
STUB
$CXX $COMMON $WORK/stubs.cpp $WORK/smoke.cpp $SOURCES -o "$WORK/smoke"
"$WORK/smoke"
echo "PASS: host smoke compile and v6 contract executable passed"
