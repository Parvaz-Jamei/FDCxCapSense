# Changelog

## 0.6.11 — Final Industrial Quality Polish

- Made `startContinuous()` preflight high-current/channel incompatibility before any Sleep Mode side-effect.
- Preserved `frefDivider` metadata in the legacy `readFrequencyHz(channel, frefHz, output)` path.
- Made `setClockDividers()` Sleep-mode guarded while keeping `writeRawClockDividers()` as the explicit raw path.
- Made automotive timestamp validation wrap-safe for `millis()` overflow.
- Prevalidated `configureSingleChannel()` arguments before entering Sleep Mode; I2C failures may leave Sleep Mode as a documented fail-safe.
- Clarified legacy liquid-level temperature metadata behavior.
- Added shared math and serialization utility headers and updated core/profile serializers to use them.

## 0.6.10 — Final Industrial RC Fix

- Made FDC2x1x `setActiveChannel()` reject active-mode channel changes.
- Made `startContinuous(newChannel)` sleep-safe by entering Sleep Mode before changing `ACTIVE_CHAN`, then restarting conversion.
- Added a real host unit-test execution script with runtime assertions for FDC2x1x startup/sleep, active-channel switching, register validation, clock-divider validation, and FDC1004 latest-vs-fresh reads.
- Updated stale AUnit frequency tests so the legacy `readFrequencyHz(channel, frefHz, output)` helper has valid `CLOCK_DIVIDERS_CHx` state when `Ok` is expected.
- Strengthened legacy documentation for the simple `readFrequencyHz()` overload; real hardware examples continue to use `readFrequencyHzFromRegisters()`.
- Updated repository metadata to `https://github.com/Parvaz-Jamei/FDCxCapSense`.
- Kept FDC211x as raw/foundation-only and did not add new sensing features or profiles.

## 0.6.8 — FDC2x1x Release Edge-Case Fix

- Fixed FDC2x1x two-channel `MUX_CONFIG.RR_SEQUENCE` validation: `b00` and `b11` are accepted for CH0/CH1, while sequences including CH2/CH3 remain rejected for FDC2112/FDC2212.
- Hardened `RESET_DEV` raw writes: reserved bits are rejected, FDC221x rejects `OUTPUT_GAIN`, and documented reset writes remain allowed.
- Added cross-register guard so `CONFIG.HIGH_CURRENT_DRV=1` cannot coexist with `MUX_CONFIG.AUTOSCAN_EN=1`; high-current remains single-channel CH0-only.
- Added `configureClockDividersForDifferential(channel, finSelMultiplier, frefDivider)` so differential sensors choose `FIN_SEL=1` or `FIN_SEL=2` explicitly.
- Kept the simple `readFrequencyHz(channel, frefHz, output)` overload as a documented legacy compatibility helper; official examples use `readFrequencyHzFromRegisters()`.
- Added `readLastCapacitancePf()` alias and clarified that `readCapacitancePf()` reads latest completed FDC1004 registers, not a fresh trigger/wait/read flow.

## 0.6.7 — FDC2x1x CONFIG Register Fix

- Corrected FDC2x1x `CONFIG` reserved-bit validation to require the writable datasheet pattern `0x1401` while preserving real option bits such as `SENSOR_ACTIVATE_SEL`, `REF_CLK_SRC`, `INTB_DIS`, and `HIGH_CURRENT_DRV`.
- Added `setReferenceClockSource()` / `referenceClockSource()` so external-clock examples do not silently assume `REF_CLK_SRC`.
- Made `configureSingleChannel()` force single-channel `MUX_CONFIG` instead of preserving previous autoscan/sequence state.
- Hardened raw `CONFIG` and `MUX_CONFIG` writes for 2-channel variants so invalid active channels and autoscan sequences are rejected.
- Added sensor-topology clock-divider helpers; the single-ended helper forces `CHx_FIN_SEL = 2`.
- Updated FDC2x1x examples to use the safe helper path and check setup status before reading data.
- Kept FDC211x documented as raw/foundation-only for v0.6.x frequency conversion and kept hardware validation owner-side.

## 0.6.6 — FDC2x1x Real-Hardware Startup Patch

- Fixed FDC2x1x `MUX_CONFIG` writes so `setDeglitch()` preserves autoscan/sequence bits and forces the datasheet/default reserved pattern `0x0208`.
- Added safe FDC2x1x startup helpers: `setSleepMode()`, `setActiveChannel()`, `startContinuous()`, `stopConversions()`, and `configureSingleChannel()`.
- Updated FDC2x1x examples to configure while asleep and start continuous conversion before waiting for data.
- Added FakeI2C FDC2x1x reset defaults and sleep/data-ready modeling for code-level tests.
- Hardened `setRCount()`, FDC211x/FDC221x capability reporting, CONFIG/MUX raw-write validation, and FDC1004 config-cache write failure behavior.
- Added docs notes for low-level FDC1004 calibration registers, single-ended high-level API scope, and FDC2x1x raw/safe helper boundaries.
- Kept hardware validation owner-side and not claimed by the Developer.

## 0.6.5 — Strict Sensor Correctness Patch

- Fixed FDC2x1x publish-blocker behavior around channel-specific data readiness, DATA MSB error flags, and clock-divider based frequency conversion.
- Added stricter FDC2x1x raw register helper validation for `CLOCK_DIVIDERS` and `DRIVE_CURRENT`.
- Made FDC2x1x examples wait for fresh data before reading frequency.
- Made FDC1004 single-shot trigger clear repeat mode and keep actual measurement-to-CIN metadata.
- Hardened sample, feature-vector, material signature, baseline, and profile edge-case validation for NaN/Inf/range issues.
- Kept hardware validation owner-side and not claimed by the Developer.


## 0.6.2 — Strict Sensor Correctness Patch

- Prepared release-grade README, Library Manager checklist, release process, maintenance plan, and public release notes.
- Added GitHub community files, issue templates, and pull request template.
- Clarified FDC2x1x `configureChannel()` as validation-only compatibility alias and added explicit register setter APIs.
- Documented `FDC2x1xLCParams.traceReferenceClockHz` as optional trace metadata for derived capacitance.
- Split chip-level INTB/DRDY capability from driver-level interrupt callback support.
- Moved historical phase notes under `docs/history/`.
- Updated hardware validation matrix for owner-side recording.
- Added Arduino Lint workflow for owner-side GitHub execution.
- Marked public repository URL and real CI execution as owner release blockers.
- Renamed FDC2x1x LC trace field to `traceReferenceClockHz` to avoid API ambiguity.
- Kept hardware validation as NOT PERFORMED by Developer.
- Made `FDCxSample::directFDC1004()` conservative for non-Ok reads so invalid samples do not carry valid-looking capacitance metadata.
- Corrected FDC1004 wrong manufacturer ID behavior to return `InvalidDeviceId`.
- Added `FDCX_DISABLE_LEGACY_PROFILE_HELPERS` gating for raw-float compatibility helper APIs.
- Added Library Manager name-uniqueness and owner-side release checklist emphasis.

## 0.5.2 — Phase 4 Release Candidate Polish

- Tightened FDC2x1x variant validation in `begin()` and calibration profile serialization.
- Corrected stale limitation/status wording after the Phase 4 FDC2x1x foundation was added.
- Kept FDC2x1x frequency-first, with FDC211x precise conversion still foundation-only and not claimed.
- Re-ran clean package, claim safety, architecture contract, and host smoke checks.

## 0.4.1 — Roadmap v6 Phase 3 Alignment

- Added `FDCxCapSense_FDC1004.h` lightweight public include.
- Added core metadata contracts: `FDCxFamily`, `FDCxMeasurementModel`, `FDCxSample`, `FDCxFeatureVector`, and `FDCxCapability`.
- Added `FDC1004Reading` and direct FDC1004 sample conversion metadata.
- Added metadata-aware official profile APIs using `FDCxFeatureVector`.
- Kept raw-float profile APIs as legacy/helper-only compatibility paths.
- Added Roadmap v6 architecture docs, profile compatibility matrix, measurement model contract, and hardware validation matrix.
- Added `FDCx_FamilyAwareProfileInput` example.
- Added v6 architecture contract tests.
- Removed generated object/build artifacts from the package.
- Kept FDC2x1x documentation/research-only; no FDC2x1x driver implementation.
