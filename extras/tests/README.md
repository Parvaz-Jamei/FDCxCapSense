# Tests

This folder contains AUnit-compatible test sketches and a Fake I2C/register simulator.

## Included tests

- `test_fdc1004_registers`: ID checks, register encoding, CAPDAC disabled/enabled paths, raw combine/sign extension.
- `test_fdc1004_errors`: I2C error, timeout, sample-rate, trigger, pF conversion.
- `test_fdc1004_phase2`: Auto-CAPDAC, baseline, reference compensation, temperature compensation, diagnostics, and profile serialization.
- `test_fdc1004_phase2_1`: power/idle helpers, temperature lookup interpolation, Auto-CAPDAC residual semantics.
- `test_fdc1004_profiles_metadata_input`: primary Roadmap v6 metadata-aware profile tests using `FDCxFeatureVector`.
- `test_v6_architecture_contracts`: rejects missing family/model, missing capacitance, derived/FDC2x1x samples, non-Ok samples, and baseline-not-learned input.
- `test_fdc1004_profile_legacy_helpers`: legacy raw-float helper coverage only. These helpers are not the official profile API.
- `test_fdc1004_profile_claim_safety`: claim-safety text guard.

## Fake I2C

`fake_i2c/FakeI2C.h` simulates FDC1004 registers and can auto-complete measurements for non-hardware tests.

## Limitations

These tests do not validate hardware, electrode design, noise, shielding, environmental stability, or real I2C bus recovery behavior.

## Host execution

`tools/host_unit_tests.sh` builds and runs a native executable with real assertions for FDC2x1x startup/sleep behavior, active-channel switching, CONFIG/MUX/ERROR_CONFIG validation, clock-divider validation, high-current no-side-effect preflight, setClockDividers active guard, frequency metadata preservation, automotive timestamp wrap-around, and FDC1004 latest-register vs fresh single-shot reads. AUnit sketches remain Arduino-compatible tests and are also syntax-checked by `tools/host_smoke_compile.sh`.
