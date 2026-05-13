#include <AUnit.h>
#include <FDCxCapSense.h>
#include "../fake_i2c/FakeI2C.h"

using namespace aunit;

test(begin_ok_with_expected_ids) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
}

test(begin_invalid_device_id) {
  FakeI2C bus;
  bus.setRegister(fdc1004_registers::kDeviceId, 0x9999);
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::InvalidDeviceId), static_cast<int>(fdc.begin(bus)));
}

test(configure_single_ended_without_capdac_uses_chb_disabled) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN1)));
  assertEqual(0x1C00, static_cast<int>(bus.getRegister(fdc1004_registers::kMeasurementConfig1)));
  assertFalse(fdc.capdacEnabled(FDC1004Measurement::M1));
}

test(configure_single_ended_with_capdac_zero_uses_chb_capdac) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureSingleEndedWithCapdac(FDC1004Measurement::M1, FDC1004Channel::CIN1, 0)));
  assertEqual(0x1000, static_cast<int>(bus.getRegister(fdc1004_registers::kMeasurementConfig1)));
  assertTrue(fdc.capdacEnabled(FDC1004Measurement::M1));
}

test(configure_single_ended_cin4_capdac_31) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureSingleEndedWithCapdac(FDC1004Measurement::M4, FDC1004Channel::CIN4, 31)));
  assertEqual(0x73E0, static_cast<int>(bus.getRegister(fdc1004_registers::kMeasurementConfig4)));
}

test(capdac_out_of_range) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.configureSingleEndedWithCapdac(FDC1004Measurement::M1, FDC1004Channel::CIN1, 32)));
}

test(reserved_register_write_is_blocked) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::InvalidArgument), static_cast<int>(fdc.writeRegister16(0x15, 0x1234)));
}

test(raw24_positive_combine) {
  FakeI2C bus;
  bus.setRegister(fdc1004_registers::kMeasurement1Msb, 0x0001);
  bus.setRegister(fdc1004_registers::kMeasurement1Lsb, 0x2300);
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  int32_t raw = 0;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readRaw24(FDC1004Measurement::M1, raw)));
  assertEqual(0x000123, static_cast<int>(raw));
}

test(raw24_negative_sign_extend) {
  FakeI2C bus;
  bus.setRegister(fdc1004_registers::kMeasurement1Msb, 0xFFFF);
  bus.setRegister(fdc1004_registers::kMeasurement1Lsb, 0xFF00);
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  int32_t raw = 0;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readRaw24(FDC1004Measurement::M1, raw)));
  assertEqual(-1, static_cast<int>(raw));
}


test(trigger_single_measurement_clears_repeat) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN1)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setRepeatMode(true)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.triggerSingleMeasurement(FDC1004Measurement::M1)));
  const uint16_t cfg = bus.getRegister(fdc1004_registers::kFdcConfig);
  assertEqual(static_cast<int>(0), static_cast<int>(cfg & fdc1004_registers::kRepeatBit));
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {}
