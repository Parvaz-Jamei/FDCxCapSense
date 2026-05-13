# Testing Strategy

## What is included

- Fake I2C/register simulator in `extras/tests/fake_i2c/`
- AUnit-compatible test sketches
- Register encoding tests
- ID-check tests
- Timeout tests
- CAPDAC disabled/enabled tests
- Raw combine/sign-extension tests
- Conversion tests
- Auto-CAPDAC tests
- Baseline tracker tests
- Reference compensation tests
- Temperature compensation tests
- Diagnostics flag tests
- Profile serialize/deserialize round-trip tests

## Important limitation

Fake-I2C tests are not hardware validation. They verify library logic, register encoding, error propagation, and algorithm bounds only.

Measurement polling is timeout-bounded. Low-level `Wire` bus hang behavior may depend on the Arduino core. Hardware validation should check I2C recovery/timeout behavior on the target board/core.

## Recommended owner-side checks

Compile at least:

- `examples/FDC1004_BasicRead/FDC1004_BasicRead.ino`
- `examples/FDC1004_AutoCapdac/FDC1004_AutoCapdac.ino`
- `examples/FDC1004_BaselineCalibration/FDC1004_BaselineCalibration.ino`
- `examples/FDC1004_TemperatureCompensation/FDC1004_TemperatureCompensation.ino`
- `extras/tests/test_fdc1004_registers/test_fdc1004_registers.ino`
- `extras/tests/test_fdc1004_errors/test_fdc1004_errors.ino`
- `extras/tests/test_fdc1004_phase2/test_fdc1004_phase2.ino`

Record board, Arduino core version, AUnit version, command, and output.
