#include <AUnit.h>
#include <FDCxCapSense.h>
#include "../fake_i2c/FakeI2C.h"

using namespace aunit;

test(read_register_i2c_error) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  bus.failRead = true;
  uint16_t value = 0;
  assertEqual(static_cast<int>(FDCxStatus::I2cError), static_cast<int>(fdc.readRegister16(fdc1004_registers::kFdcConfig, value)));
}

test(wait_for_measurement_timeout) {
  FakeI2C bus;
  bus.autoCompleteMeasurement[0] = false;
  bus.setRegister(fdc1004_registers::kFdcConfig, static_cast<uint16_t>(static_cast<uint16_t>(FDC1004Rate::SPS_100) << 10));
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Timeout), static_cast<int>(fdc.waitForMeasurement(FDC1004Measurement::M1, 3)));
}

test(wait_for_measurement_done) {
  FakeI2C bus;
  bus.setRegister(fdc1004_registers::kFdcConfig, 0x0008);
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.waitForMeasurement(FDC1004Measurement::M1, 3)));
}

test(sample_rate_encoding) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.setSampleRate(FDC1004Rate::SPS_400)));
  assertEqual(0x0C00, static_cast<int>(bus.getRegister(fdc1004_registers::kFdcConfig) & fdc1004_registers::kRateMask));
}

test(trigger_measurement_1_sets_bit_7) {
  FakeI2C bus;
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.triggerSingleMeasurement(FDC1004Measurement::M1)));
  assertEqual(0x0080, static_cast<int>(bus.getRegister(fdc1004_registers::kFdcConfig) & fdc1004_registers::kMeasurementTriggerMask));
}

test(conversion_pf_with_capdac_offset) {
  FakeI2C bus;
  bus.setRegister(fdc1004_registers::kMeasurement1Msb, 0x0008);
  bus.setRegister(fdc1004_registers::kMeasurement1Lsb, 0x0000);
  FDC1004 fdc;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.begin(bus)));
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.configureSingleEndedWithCapdac(FDC1004Measurement::M1, FDC1004Channel::CIN1, 2)));
  float pf = 0.0f;
  assertEqual(static_cast<int>(FDCxStatus::Ok), static_cast<int>(fdc.readCapacitancePf(FDC1004Measurement::M1, pf)));
  assertNear(6.25390625, pf, 0.0001);
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {}
