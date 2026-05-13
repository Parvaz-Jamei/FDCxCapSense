#include <AUnit.h>
#include <FDCxCapSense.h>

test(deserialize_rejects_corrupt_magic) {
  FDC1004CalibrationProfile p;
  uint8_t b[kFDC1004CalibrationProfileSerializedSize] = {};
  assertTrue(fdcxSerializeProfile(p, b, sizeof(b)));
  b[0] ^= 0x01u;
  FDC1004CalibrationProfile out;
  assertFalse(fdcxDeserializeProfile(b, sizeof(b), out));
}

test(deserialize_rejects_corrupt_version) {
  FDC1004CalibrationProfile p;
  uint8_t b[kFDC1004CalibrationProfileSerializedSize] = {};
  assertTrue(fdcxSerializeProfile(p, b, sizeof(b)));
  b[4] ^= 0x01u;
  FDC1004CalibrationProfile out;
  assertFalse(fdcxDeserializeProfile(b, sizeof(b), out));
}

test(serialize_rejects_invalid_fields) {
  FDC1004CalibrationProfile p;
  uint8_t b[kFDC1004CalibrationProfileSerializedSize] = {};
  p.capdacCode[0] = 32;
  assertFalse(fdcxSerializeProfile(p, b, sizeof(b)));
}

void setup() {}
void loop() { aunit::TestRunner::run(); }
