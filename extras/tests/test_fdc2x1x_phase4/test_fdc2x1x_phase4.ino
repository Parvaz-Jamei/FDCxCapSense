#include <AUnit.h>
#include <FDCxCapSense.h>
#include "../fake_i2c/FakeI2C.h"

static void seedFdc2Ids(FakeI2C& bus, FDC2x1xVariant variant) {
  bus.expectedAddress = fdc2x1x_registers::kDefaultI2cAddressLow;
  bus.setRegister(fdc2x1x_registers::kManufacturerId, fdc2x1x_registers::kExpectedManufacturerId);
  bus.setRegister(fdc2x1x_registers::kDeviceId,
                  fdc2x1xVariantIsFdc211x(variant) ? fdc2x1x_registers::kExpectedDeviceIdFdc211x
                                                    : fdc2x1x_registers::kExpectedDeviceIdFdc221x);
}

static FDC2x1xCalibrationProfile validProfile(FDC2x1xVariant variant = FDC2x1xVariant::FDC2214) {
  FDC2x1xCalibrationProfile profile;
  profile.variant = variant;
  profile.chip = (variant == FDC2x1xVariant::FDC2112) ? FDCxChipId::FDC2112 :
                 (variant == FDC2x1xVariant::FDC2114) ? FDCxChipId::FDC2114 :
                 (variant == FDC2x1xVariant::FDC2212) ? FDCxChipId::FDC2212 : FDCxChipId::FDC2214;
  profile.referenceClockHz = 40000000.0f;
  for (uint8_t i = 0; i < 4; ++i) {
    profile.inductanceUH[i] = (i < fdc2x1xVariantChannelCount(variant)) ? 18.0f : 0.0f;
    profile.parasiticCapacitancePf[i] = 2.0f;
    profile.baselineFrequencyHz[i] = (i < fdc2x1xVariantChannelCount(variant)) ? 1000000.0f : 0.0f;
  }
  return profile;
}

test(FDC2x1xVariantChannelGuard) {
  assertTrue(fdc2x1xChannelIsValid(FDC2x1xVariant::FDC2112, FDC2x1xChannel::CH0));
  assertFalse(fdc2x1xChannelIsValid(FDC2x1xVariant::FDC2112, FDC2x1xChannel::CH2));
  assertFalse(fdc2x1xChannelIsValid(FDC2x1xVariant::FDC2212, FDC2x1xChannel::CH3));
  assertTrue(fdc2x1xChannelIsValid(FDC2x1xVariant::FDC2114, FDC2x1xChannel::CH3));
  assertTrue(fdc2x1xChannelIsValid(FDC2x1xVariant::FDC2214, FDC2x1xChannel::CH3));
}

test(FDC2x1xBeginIdentityChecks) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  bus.setRegister(fdc2x1x_registers::kManufacturerId, 0x0000);
  assertEqual(static_cast<int>(FDCxStatus::InvalidDeviceId), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  bus.setRegister(fdc2x1x_registers::kManufacturerId, fdc2x1x_registers::kExpectedManufacturerId);
  bus.setRegister(fdc2x1x_registers::kDeviceId, fdc2x1x_registers::kExpectedDeviceIdFdc211x);
  assertEqual(static_cast<int>(FDCxStatus::InvalidDeviceId), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
}

test(FDC2x1xRejectsInvalidVariant) {
  FakeI2C bus;
  bus.expectedAddress = fdc2x1x_registers::kDefaultI2cAddressLow;
  bus.setRegister(fdc2x1x_registers::kManufacturerId, fdc2x1x_registers::kExpectedManufacturerId);
  bus.setRegister(fdc2x1x_registers::kDeviceId, fdc2x1x_registers::kExpectedDeviceIdFdc221x);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.begin(bus, static_cast<FDC2x1xVariant>(99))));
}


test(FDC2x1xReadRawVariantWidth) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0ABC);
  bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x1234);
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh0UnreadMask);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  uint32_t raw = 0;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readRaw(FDC2x1xChannel::CH0, raw)));
  assertEqual(static_cast<uint32_t>(0x0ABC1234UL), raw);

  FakeI2C bus2114;
  seedFdc2Ids(bus2114, FDC2x1xVariant::FDC2114);
  bus2114.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0A55);
  bus2114.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0xFFFF);
  FDC2x1x fdc2114;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2114.begin(bus2114, FDC2x1xVariant::FDC2114)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2114.readRaw(FDC2x1xChannel::CH0, raw)));
  assertEqual(static_cast<uint32_t>(0x0A55UL), raw);
  assertEqual(static_cast<uint16_t>(0), bus2114.readHits[fdc2x1x_registers::dataLsbRegister(0)]);
}

test(FDC2x1xWaitDataReadyRequiresReadyBit) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  bus.setRegister(fdc2x1x_registers::kStatus, 0x0000);
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 1)));
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh0UnreadMask);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 10)));
}

test(FDC2x1xRegisterWriteGuard) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2212);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2212)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kStatus, 0xFFFF)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::dataMsbRegister(0), 0x0000)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::driveCurrentRegister(3), 0x0001)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::driveCurrentRegister(1), 0x0001)));
}

test(FDC2x1xSetDeglitchPropagatesReadError) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  bus.failRead = true;
  assertEqual(static_cast<int>(FDCxStatus::I2cError), static_cast<int>(fdc.setDeglitch(FDC2x1xDeglitch::MHz_10)));
  assertEqual(static_cast<uint16_t>(0), bus.writeHits[fdc2x1x_registers::kMuxConfig]);
}

test(FDC2x1xFrequencyAndIdle) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800);
  bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000);
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh0UnreadMask);
  bus.setRegister(fdc2x1x_registers::clockDividersRegister(0),
                  static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(2) | 1u));
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  FDC2x1xFrequency frequency;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, frequency)));
  assertEqual(static_cast<int>(FDCxMeasurementModel::ResonantFrequency), static_cast<int>(frequency.toSample().model));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.idle()));

  FakeI2C bus2112;
  seedFdc2Ids(bus2112, FDC2x1xVariant::FDC2112);
  FDC2x1x fdc2112;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2112.begin(bus2112, FDC2x1xVariant::FDC2112)));
  assertEqual(static_cast<int>(FDCxStatus::NotSupported), static_cast<int>(fdc2112.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, frequency)));
}


test(FDC2x1xReadFrequencyHzRequiresValidClockDividers) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800);
  bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000);
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh0UnreadMask);
  bus.setRegister(fdc2x1x_registers::clockDividersRegister(0), 0u);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  FDC2x1xFrequency frequency;
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, frequency)));

  bus.setRegister(fdc2x1x_registers::clockDividersRegister(0),
                  static_cast<uint16_t>(fdc2x1x_registers::finSelBitsForMultiplier(2) | 1u));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, frequency)));
}

test(FDC2x1xDerivedSampleMetadata) {
  FDC2x1xFrequency frequency;
  frequency.status = FDCxStatus::Ok;
  frequency.frequencyHz = 1000000.0f;
  FDC2x1xLCParams params;
  params.inductanceUH = 18.0f;
  params.parasiticCapacitancePf = 2.0f;
  params.traceReferenceClockHz = 40000000.0f;
  FDC2x1xDerivedCapacitance cap;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2x1xDeriveCapacitancePf(frequency, params, cap)));
  FDCxSample sample = cap.toSample();
  assertEqual(static_cast<int>(FDCxFamily::FDC2x1x_ResonantLC), static_cast<int>(sample.family));
  assertEqual(static_cast<int>(FDCxMeasurementModel::DerivedCapacitance), static_cast<int>(sample.model));
  assertFalse(sample.capacitanceIsDirect);
  assertTrue(sample.capacitanceIsDerived);
}

test(FDC1004ProfilesRejectFDC2x1xDerived) {
  FDC2x1xDerivedCapacitance cap;
  cap.status = FDCxStatus::Ok;
  cap.capacitancePf = 3.0f;
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(cap.toSample(), 2.0f, 0.02f, 0.0f, 0, true);
  FDCxSurfaceStateConfig surfaceConfig;
  FDCxSurfaceStateProfile surface(surfaceConfig);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(surface.update(features).status));
}


test(FDC2x1xUnknownVariantIsNotFourChannel) {
  const FDC2x1xVariant unknown = static_cast<FDC2x1xVariant>(99);
  assertFalse(fdc2x1xVariantIsKnown(unknown));
  assertEqual(static_cast<uint8_t>(0), fdc2x1xVariantChannelCount(unknown));
  assertFalse(fdc2x1xChannelIsValid(unknown, FDC2x1xChannel::CH0));
}

test(FDC2x1xToSampleInvalidStatusIsSafe) {
  FDC2x1xFrequency frequency;
  frequency.status = FDCxStatus::Timeout;
  frequency.frequencyHz = 12345.0f;
  FDCxSample f = frequency.toSample();
  assertFalse(f.hasFrequency);
  assertEqual(static_cast<float>(0.0f), f.frequencyHz);

  FDC2x1xDerivedCapacitance cap;
  cap.status = FDCxStatus::InvalidArgument;
  cap.capacitancePf = 10.0f;
  FDCxSample c = cap.toSample();
  assertFalse(c.hasCapacitance);
  assertFalse(c.capacitanceIsDerived);
  assertEqual(static_cast<float>(0.0f), c.capacitancePf);
}

test(FDC2x1xDerivedCapacitanceRejectsInvalidResults) {
  FDC2x1xFrequency frequency;
  frequency.status = FDCxStatus::Ok;
  frequency.frequencyHz = 1000000.0f;
  FDC2x1xLCParams params;
  params.inductanceUH = 18.0f;
  params.parasiticCapacitancePf = 999999999.0f;
  params.traceReferenceClockHz = 40000000.0f;
  FDC2x1xDerivedCapacitance cap;
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc2x1xDeriveCapacitancePf(frequency, params, cap)));
  params.parasiticCapacitancePf = -1.0f;
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc2x1xDeriveCapacitancePf(frequency, params, cap)));
}


test(FDC2x1xCalibrationProfileSerialization) {
  FDC2x1xCalibrationProfile profile = validProfile(FDC2x1xVariant::FDC2214);
  uint8_t buffer[kFDC2x1xCalibrationProfileSerializedSize] = {};
  assertTrue(fdc2x1xSerializeProfile(profile, buffer, sizeof(buffer)));
  FDC2x1xCalibrationProfile parsed;
  assertTrue(fdc2x1xDeserializeProfile(buffer, sizeof(buffer), parsed));
  assertEqual(static_cast<int>(FDCxFamily::FDC2x1x_ResonantLC), static_cast<int>(parsed.family));
  assertEqual(static_cast<int>(FDC2x1xVariant::FDC2214), static_cast<int>(parsed.variant));
  buffer[0] ^= 0x01u;
  assertFalse(fdc2x1xDeserializeProfile(buffer, sizeof(buffer), parsed));
  profile.chip = FDCxChipId::FDC1004;
  assertFalse(fdc2x1xSerializeProfile(profile, buffer, sizeof(buffer)));
  profile = validProfile(FDC2x1xVariant::FDC2214);
  profile.inductanceUH[0] = -1.0f;
  assertFalse(fdc2x1xSerializeProfile(profile, buffer, sizeof(buffer)));
  profile = validProfile(FDC2x1xVariant::FDC2214);
  profile.variant = static_cast<FDC2x1xVariant>(99);
  profile.chip = FDCxChipId::Unknown;
  assertFalse(fdc2x1xSerializeProfile(profile, buffer, sizeof(buffer)));
  profile = validProfile(FDC2x1xVariant::FDC2214);
  profile.parasiticCapacitancePf[0] = -0.1f;
  assertFalse(fdc2x1xSerializeProfile(profile, buffer, sizeof(buffer)));
  profile = validProfile(FDC2x1xVariant::FDC2214);
  profile.baselineFrequencyHz[0] = 0.0f;
  assertFalse(fdc2x1xSerializeProfile(profile, buffer, sizeof(buffer)));

}


test(FDC2x1xFrequencyFromClockDividersFinSel) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800);
  bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000);
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh0UnreadMask);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureClockDividers(FDC2x1xChannel::CH0, 1, 2)));
  FDC2x1xFrequency out1;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, out1)));
  assertEqual(static_cast<uint8_t>(1), out1.finSelMultiplier);
  assertEqual(static_cast<uint16_t>(2), out1.frefDivider);
  assertNear(10000000.0f, out1.frequencyHz, 1.0f);

  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureClockDividers(FDC2x1xChannel::CH0, 2, 2)));
  FDC2x1xFrequency out2;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, out2)));
  assertEqual(static_cast<uint8_t>(2), out2.finSelMultiplier);
  assertNear(20000000.0f, out2.frequencyHz, 1.0f);

  bus.setRegister(fdc2x1x_registers::clockDividersRegister(0), fdc2x1x_registers::finSelBitsForMultiplier(1));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, out1)));
  bus.setRegister(fdc2x1x_registers::clockDividersRegister(0), static_cast<uint16_t>(3u << fdc2x1x_registers::kClockFinSelShift) | 2u);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, out1)));
}

test(FDC2x1xWaitDataReadyIsChannelSpecificAndErrors) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh1UnreadMask);
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 1)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH1, 10)));
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusErrorMask | fdc2x1x_registers::kStatusCh0UnreadMask);
  assertEqual(static_cast<int>(FDCxStatus::MeasurementNotReady), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 10)));
}

test(FDC2x1xReadRawRejectsDataErrorFlags) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  uint32_t raw = 0;
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), fdc2x1x_registers::kDataErrWdMask | 0x0001u);
  assertEqual(static_cast<int>(FDCxStatus::MeasurementNotReady), static_cast<int>(fdc.readRaw(FDC2x1xChannel::CH0, raw)));
  assertEqual(static_cast<uint32_t>(0), raw);
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), fdc2x1x_registers::kDataErrAwMask | 0x0001u);
  assertEqual(static_cast<int>(FDCxStatus::MeasurementNotReady), static_cast<int>(fdc.readRaw(FDC2x1xChannel::CH0, raw)));
}

test(FDC1004ReadingReportsActualCinAndRejectsUnconfigured) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  FDC1004Reading reading;
  assertEqual(static_cast<int>(FDCxStatus::MeasurementNotReady), static_cast<int>(fdc.readCapacitanceReading(FDC1004Measurement::M1, reading)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN4)));
  bus.setSimulatedCapacitance(0, 1.0f);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readCapacitanceReading(FDC1004Measurement::M1, reading)));
  assertEqual(static_cast<uint8_t>(static_cast<uint8_t>(FDC1004Channel::CIN4)), reading.channel);
}

test(ProfileEdgeCasesAndMaterialReference) {
  FDCxSample sample = FDCxSample::directFDC1004(0, 1.0f, FDCxStatus::Ok, 0, 1.0f);
  FDCxFeatureVector features = FDCxFeatureVector::fromSample(sample, 0.000001f, 0.01f, 0.0f, 0, true);
  assertEqual(static_cast<float>(0.0f), features.normalizedDelta);

  FDCxMaterialSignature signature;
  features.hasReference = false;
  features.referenceDeltaPf = 0.5f;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(computeMaterialSignature(features, signature)));
  assertEqual(static_cast<float>(0.0f), signature.referenceRatio);

  FDCxSurfaceStateConfig badSurface;
  badSurface.maxNoisePf = -1.0f;
  FDCxSurfaceStateProfile surface(badSurface);
  features.temperatureC = -1.0f;
  features.hasTemperature = true;
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(surface.update(features).status));
}


test(FDC2x1xSafeRegisterHelpers) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.configureClockDividers(FDC2x1xChannel::CH0, 3, 1)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.configureClockDividers(FDC2x1xChannel::CH0, 1, 0)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.writeRawClockDividers(FDC2x1xChannel::CH0, 0x0400)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.writeRawDriveCurrent(FDC2x1xChannel::CH0, 0x0001)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.setDriveCurrentCode(FDC2x1xChannel::CH0, 32)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setDriveCurrentCode(FDC2x1xChannel::CH0, 15)));
}

test(FDC2x1xReadFrequencyRequiresFreshRequestedChannel) {
  FakeI2C bus;
  seedFdc2Ids(bus, FDC2x1xVariant::FDC2214);
  bus.setRegister(fdc2x1x_registers::dataMsbRegister(0), 0x0800);
  bus.setRegister(fdc2x1x_registers::dataLsbRegister(0), 0x0000);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  FDC2x1xFrequency out;
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh1UnreadMask);
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, out)));
  assertFalse(out.dataReady);
  bus.setRegister(fdc2x1x_registers::kStatus, fdc2x1x_registers::kStatusCh0UnreadMask);
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readFrequencyHz(FDC2x1xChannel::CH0, 40000000.0f, out)));
  assertTrue(out.dataReady);
}


test(FDC2x1xSetDeglitchPreservesMuxPatternAndAutoscan) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  bus.setRegister(fdc2x1x_registers::kMuxConfig,
                  static_cast<uint16_t>(0xA000u | fdc2x1x_registers::kMuxConfigReservedPattern | 0x0001u));
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setDeglitch(FDC2x1xDeglitch::MHz_10)));
  assertEqual(static_cast<uint16_t>(0xA000u | fdc2x1x_registers::kMuxConfigReservedPattern |
                                    static_cast<uint16_t>(FDC2x1xDeglitch::MHz_10)),
              bus.getRegister(fdc2x1x_registers::kMuxConfig));
  assertFalse(bus.badMuxConfigWrite);
}

test(FDC2x1xSafeStartupFlowAndSleepModel) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<uint16_t>(0x2801u), bus.getRegister(fdc2x1x_registers::kConfig));
  assertEqual(static_cast<uint16_t>(0x020Fu), bus.getRegister(fdc2x1x_registers::kMuxConfig));
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 1)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setSleepMode(false)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 10)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setSleepMode(true)));
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 1)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.startContinuous(FDC2x1xChannel::CH1)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH1, 10)));
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 1)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.stopConversions())) ;
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH1, 1)));
}

test(FDC2x1xStartContinuousRejectsInvalidChannelFor2chVariant) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2212);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2212)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.startContinuous(FDC2x1xChannel::CH2)));
}

test(FDC2x1xRawSensitiveRegistersRejectBadReservedBits) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0x0005u)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kConfig, 0x0000u)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0x020Du)));
}

test(FDC2x1xSetRCountRejectsReservedLowValues) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.setRCount(FDC2x1xChannel::CH0, 0x00FFu)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setRCount(FDC2x1xChannel::CH0, 0x0100u)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.writeRawRCount(FDC2x1xChannel::CH0, 0x0001u)));
}

test(FDC2x1xCapabilityDistinguishesFDC211xAndFDC221x) {
  FakeI2C bus2214;
  bus2214.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc2214;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2214.begin(bus2214, FDC2x1xVariant::FDC2214)));
  FDCxCapability caps2214 = fdc2214.capability();
  assertTrue(caps2214.chipUsesResonantFrequency);
  assertTrue(caps2214.driverSupportsFrequencyConversion);
  assertTrue(caps2214.supportsResonantFrequency);

  FakeI2C bus2114;
  bus2114.resetFdc2x1x(FDC2x1xVariant::FDC2114);
  FDC2x1x fdc2114;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2114.begin(bus2114, FDC2x1xVariant::FDC2114)));
  FDCxCapability caps2114 = fdc2114.capability();
  assertTrue(caps2114.chipUsesResonantFrequency);
  assertFalse(caps2114.driverSupportsFrequencyConversion);
  assertFalse(caps2114.supportsResonantFrequency);
}

test(FDC2x1xConfigureSingleChannelStartsConversion) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureSingleChannel(FDC2x1xChannel::CH0,
                                                                                             0x0100u,
                                                                                             0x0100u,
                                                                                             2,
                                                                                             1,
                                                                                             15,
                                                                                             FDC2x1xDeglitch::MHz_10)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.waitDataReady(FDC2x1xChannel::CH0, 10)));
}


test(FDC2x1xConfigReservedMaskMatchesDatasheetWritablePattern) {
  assertFalse(fdc2x1x_registers::configReservedPatternIsValid(0x0801u));
  assertTrue(fdc2x1x_registers::configReservedPatternIsValid(0x1401u));
  assertTrue(fdc2x1x_registers::configReservedPatternIsValid(0x1601u));
  assertTrue(fdc2x1x_registers::configReservedPatternIsValid(0x1E01u));
  assertFalse(fdc2x1x_registers::configReservedPatternIsValid(0x1501u));
  assertFalse(fdc2x1x_registers::configReservedPatternIsValid(0x1001u));
  assertFalse(fdc2x1x_registers::configReservedPatternIsValid(0x0401u));
}

test(FDC2x1xReferenceClockAndConfigOptionsArePreserved) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.setReferenceClockSource(FDC2x1xReferenceClockSource::ExternalClockInput)));
  assertTrue((bus.getRegister(fdc2x1x_registers::kConfig) & fdc2x1x_registers::kConfigRefClockSrcMask) != 0u);
  assertTrue(fdc2x1x_registers::configReservedPatternIsValid(bus.getRegister(fdc2x1x_registers::kConfig)));

  FDC2x1xReferenceClockSource source = FDC2x1xReferenceClockSource::InternalOscillator;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.referenceClockSource(source)));
  assertEqual(static_cast<int>(FDC2x1xReferenceClockSource::ExternalClockInput), static_cast<int>(source));

  bus.setRegister(fdc2x1x_registers::kConfig,
                  static_cast<uint16_t>(fdc2x1x_registers::kConfigReservedRequiredOnes |
                                        fdc2x1x_registers::kConfigSensorActivateMask |
                                        fdc2x1x_registers::kConfigRefClockSrcMask |
                                        fdc2x1x_registers::kConfigIntbDisableMask |
                                        fdc2x1x_registers::kConfigHighCurrentMask));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setSleepMode(true)));
  const uint16_t slept = bus.getRegister(fdc2x1x_registers::kConfig);
  assertTrue((slept & fdc2x1x_registers::kConfigRefClockSrcMask) != 0u);
  assertTrue((slept & fdc2x1x_registers::kConfigSensorActivateMask) != 0u);
  assertTrue((slept & fdc2x1x_registers::kConfigIntbDisableMask) != 0u);
  assertTrue((slept & fdc2x1x_registers::kConfigHighCurrentMask) != 0u);
  assertTrue((slept & fdc2x1x_registers::kConfigSleepModeMask) != 0u);

  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.startContinuous(FDC2x1xChannel::CH0)));
  const uint16_t running = bus.getRegister(fdc2x1x_registers::kConfig);
  assertTrue((running & fdc2x1x_registers::kConfigRefClockSrcMask) != 0u);
  assertFalse((running & fdc2x1x_registers::kConfigSleepModeMask) != 0u);
}

test(FDC2x1xReferenceClockCanReturnToInternal) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.setReferenceClockSource(FDC2x1xReferenceClockSource::ExternalClockInput)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.setReferenceClockSource(FDC2x1xReferenceClockSource::InternalOscillator)));
  assertFalse((bus.getRegister(fdc2x1x_registers::kConfig) & fdc2x1x_registers::kConfigRefClockSrcMask) != 0u);
}

test(FDC2x1xConfigureSingleChannelForcesSingleChannelMux) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  bus.setRegister(fdc2x1x_registers::kMuxConfig,
                  static_cast<uint16_t>(0xE000u | fdc2x1x_registers::kMuxConfigReservedPattern |
                                        static_cast<uint16_t>(FDC2x1xDeglitch::MHz_1)));
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.configureSingleChannel(FDC2x1xChannel::CH0,
                                                          0x0100u,
                                                          0x0100u,
                                                          FDC2x1xSensorTopology::SingleEnded,
                                                          1,
                                                          15,
                                                          FDC2x1xDeglitch::MHz_10)));
  const uint16_t mux = bus.getRegister(fdc2x1x_registers::kMuxConfig);
  assertEqual(static_cast<uint16_t>(0), static_cast<uint16_t>(mux & fdc2x1x_registers::kMuxConfigAutoscanSequenceMask));
  assertEqual(static_cast<uint16_t>(fdc2x1x_registers::kMuxConfigReservedPattern |
                                    static_cast<uint16_t>(FDC2x1xDeglitch::MHz_10)), mux);
}

test(FDC2x1xConfigRawWriteIsVariantAware) {
  FakeI2C bus2212;
  bus2212.resetFdc2x1x(FDC2x1xVariant::FDC2212);
  FDC2x1x fdc2212;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2212.begin(bus2212, FDC2x1xVariant::FDC2212)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc2212.writeRegister16(fdc2x1x_registers::kConfig,
                                                       fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH2, false))));

  FakeI2C bus2112;
  bus2112.resetFdc2x1x(FDC2x1xVariant::FDC2112);
  FDC2x1x fdc2112;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2112.begin(bus2112, FDC2x1xVariant::FDC2112)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc2112.writeRegister16(fdc2x1x_registers::kConfig,
                                                       fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH3, false))));

  FakeI2C bus2214;
  bus2214.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc2214;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2214.begin(bus2214, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc2214.writeRegister16(fdc2x1x_registers::kConfig,
                                                       fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH3, false))));
}

test(FDC2x1xMuxRawWriteIsVariantAware) {
  FakeI2C bus2212;
  bus2212.resetFdc2x1x(FDC2x1xVariant::FDC2212);
  FDC2x1x fdc2212;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2212.begin(bus2212, FDC2x1xVariant::FDC2212)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc2212.writeRegister16(fdc2x1x_registers::kMuxConfig, 0xA20Du)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc2212.writeRegister16(fdc2x1x_registers::kMuxConfig, 0xC20Du)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc2212.writeRegister16(fdc2x1x_registers::kMuxConfig, 0x820Du)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc2212.writeRegister16(fdc2x1x_registers::kMuxConfig, 0xE20Du)));

  FakeI2C bus2214;
  bus2214.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc2214;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2214.begin(bus2214, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc2214.writeRegister16(fdc2x1x_registers::kMuxConfig, 0xC20Du)));
}

test(FDC2x1xSingleEndedClockDividerHelperForcesFinSelTwo) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.configureClockDividersForSingleEnded(FDC2x1xChannel::CH0, 2)));
  uint8_t multiplier = 0;
  assertTrue(fdc2x1x_registers::decodeFinSelMultiplier(bus.getRegister(fdc2x1x_registers::clockDividersRegister(0)), multiplier));
  assertEqual(static_cast<uint8_t>(2), multiplier);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.configureClockDividers(FDC2x1xChannel::CH0, FDC2x1xSensorTopology::Differential, 2)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.configureClockDividersForDifferential(FDC2x1xChannel::CH0, 1, 2)));
  assertTrue(fdc2x1x_registers::decodeFinSelMultiplier(bus.getRegister(fdc2x1x_registers::clockDividersRegister(0)), multiplier));
  assertEqual(static_cast<uint8_t>(1), multiplier);
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.configureClockDividersForDifferential(FDC2x1xChannel::CH0, 2, 2)));
  assertTrue(fdc2x1x_registers::decodeFinSelMultiplier(bus.getRegister(fdc2x1x_registers::clockDividersRegister(0)), multiplier));
  assertEqual(static_cast<uint8_t>(2), multiplier);
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.configureClockDividersForDifferential(FDC2x1xChannel::CH0, 3, 2)));
}


test(FDC2x1xResetDevWriteValidation) {
  FakeI2C bus2214;
  bus2214.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc2214;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2214.begin(bus2214, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc2214.writeRegister16(fdc2x1x_registers::kResetDev,
                                                       fdc2x1x_registers::kResetDevOutputGainMask)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc2214.writeRegister16(fdc2x1x_registers::kResetDev, 0x0001u)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc2214.writeRegister16(fdc2x1x_registers::kResetDev,
                                                       fdc2x1x_registers::kResetDevResetMask)));
  assertEqual(static_cast<uint16_t>(0x2801u), bus2214.getRegister(fdc2x1x_registers::kConfig));

  FakeI2C bus2114;
  bus2114.resetFdc2x1x(FDC2x1xVariant::FDC2114);
  FDC2x1x fdc2114;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc2114.begin(bus2114, FDC2x1xVariant::FDC2114)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc2114.writeRegister16(fdc2x1x_registers::kResetDev, 0x0600u)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc2114.writeRegister16(fdc2x1x_registers::kResetDev, 0x0800u)));
}

test(FDC2x1xHighCurrentAndAutoscanCrossGuard) {
  FakeI2C bus;
  bus.resetFdc2x1x(FDC2x1xVariant::FDC2214);
  FDC2x1x fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus, FDC2x1xVariant::FDC2214)));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0x820Du)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kConfig,
                  static_cast<uint16_t>(fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH0, false) |
                                        fdc2x1x_registers::kConfigHighCurrentMask))));

  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kMuxConfig,
                                                   fdc2x1x_registers::singleChannelMuxValue(FDC2x1xDeglitch::MHz_10))));
  assertEqual(static_cast<int>(FDCxStatus::Ok),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kConfig,
                  static_cast<uint16_t>(fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH0, false) |
                                        fdc2x1x_registers::kConfigHighCurrentMask))));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kMuxConfig, 0x820Du)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument),
              static_cast<int>(fdc.writeRegister16(fdc2x1x_registers::kConfig,
                  static_cast<uint16_t>(fdc2x1x_registers::configValueFor(FDC2x1xChannel::CH1, false) |
                                        fdc2x1x_registers::kConfigHighCurrentMask))));
}

void setup() {}
void loop() { aunit::TestRunner::run(); }
