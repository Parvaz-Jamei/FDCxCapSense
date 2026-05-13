# FDCxCapSense

**Arduino library for TI FDC1004, FDC1004-Q1 and FDC2x1x capacitive sensing ICs.**

<p align="center">
  <b>Calibration-first • Clean-room • Family-aware • Arduino-friendly</b><br>
  FDC1004 direct capacitance sensing + FDC2x1x frequency-first resonant sensing
</p>

<p align="center">
  <img alt="Arduino Library" src="https://img.shields.io/badge/Arduino-Library-00979D">
  <img alt="Category" src="https://img.shields.io/badge/category-Sensors-blue">
  <img alt="License" src="https://img.shields.io/badge/license-MIT-green">
  <img alt="Status" src="https://img.shields.io/badge/status-Public%20Release%20Candidate-orange">
  <img alt="Hardware" src="https://img.shields.io/badge/hardware%20validation-owner--side-lightgrey">
</p>

---

## What is FDCxCapSense?

**FDCxCapSense** is a clean-room Arduino library for **TI FDC capacitive sensing ICs**, designed for research, prototyping, and reference-style embedded sensing workflows.

It supports:

- **FDC1004 / FDC1004-Q1** direct capacitance sensing
- **FDC2112 / FDC2114 / FDC2212 / FDC2214** frequency-first resonant sensing
- CAPDAC calibration
- baseline tracking
- diagnostics
- temperature compensation helpers
- metadata-aware samples
- experimental profile helpers for liquid level, proximity, material response, powder/granular proxy sensing, and surface-state candidates

The library is designed to keep **FDC1004 direct CDC physics** separate from **FDC2x1x resonant LC physics**.

> Not just a breakout driver — a calibration-first, family-aware Arduino reference library for TI FDC capacitive sensing ICs.

---

## Why this library exists

This project started as part of **Proio** research work around capacitive sensing for icing and surface-state sensor experiments.

Website: [proio.ir](https://proio.ir)

While developing sensor research tools for our own work, we decided to publish this library freely as a more complete reference-style Arduino library for the developer, embedded systems, and sensing community.

We welcome feedback, bug reports, test results, documentation improvements, and better workflow suggestions. If you find an issue, an unclear API, a datasheet mismatch, or hardware behavior that needs better handling, please open an issue so we can improve the project together.

Maintainer: **Parvaz Jamei**  
LinkedIn: [linkedin.com/in/parvaz-jamei](https://www.linkedin.com/in/parvaz-jamei)

---

## Key advantages

| Area | What FDCxCapSense provides |
|---|---|
| **FDC1004 support** | Direct capacitance readout, CAPDAC handling, baseline tracking, and diagnostics |
| **FDC2x1x support** | Frequency-first foundation for resonant LC sensing with model-dependent derived capacitance |
| **Family-aware design** | FDC1004 and FDC2x1x are not treated as the same physical measurement model |
| **Calibration-first workflow** | Designed around baseline, drift, temperature, reference compensation, and profile metadata |
| **Arduino-friendly structure** | Lightweight per-family headers and standard Arduino Library layout |
| **Claim-safe profiles** | Experimental helpers with confidence, reason, and raw metrics; no hardcoded magic detection |
| **Clean-room policy** | Built independently with strict third-party code reuse rules |
| **Research-ready** | Useful for liquid level, proximity, material response, surface-state experiments, and industrial IoT prototypes |

---

## Supported chips

| Chip family | Status | Measurement model |
|---|---:|---|
| FDC1004 | Supported | Direct capacitance-to-digital conversion |
| FDC1004-Q1 | Supported at API/docs level | Direct capacitance-to-digital conversion |
| FDC2212 | Frequency-first foundation | Resonant LC / frequency-based sensing |
| FDC2214 | Frequency-first foundation | Resonant LC / frequency-based sensing |
| FDC2112 | Raw/foundation support | 12-bit raw foundation; precise conversion is not implemented in v0.6.x |
| FDC2114 | Raw/foundation support | 12-bit raw foundation; precise conversion is not implemented in v0.6.x |

> FDC2x1x capacitance values are **derived/model-dependent** and require valid LC parameters. They are not the same as FDC1004 direct pF readings.

---

## Installation

### Arduino IDE

1. Open Arduino IDE.
2. Go to **Sketch → Include Library → Manage Libraries**.
3. Search for:

```text
FDCxCapSense
```

Arduino Library Manager submission is pending owner-side public repository URL, CI, Arduino Lint, release tagging, and final owner review.

### Manual installation

Download the release ZIP and place it in your Arduino libraries folder.

---

## Include options

Use the lightweight family-specific headers when possible:

```cpp
#include <FDCxCapSense_FDC1004.h>
```

```cpp
#include <FDCxCapSense_FDC2x1x.h>
```

Use the full header only when you need all features and experimental profile helpers:

```cpp
#include <FDCxCapSense.h>
```

`library.properties` intentionally controls the Arduino IDE Include Library suggestions with:

```text
includes=FDCxCapSense.h,FDCxCapSense_FDC1004.h,FDCxCapSense_FDC2x1x.h
```

---

## Quick Start — FDC1004 Basic Read

```cpp
#include <Wire.h>
#include <FDCxCapSense_FDC1004.h>

FDC1004 fdc;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  FDCxStatus status = fdc.begin(Wire);
  if (status != FDCxStatus::Ok) {
    Serial.println("FDC1004 not detected");
    return;
  }

  fdc.setSampleRate(FDC1004Rate::SPS_100);
  fdc.configureSingleEnded(FDC1004Measurement::M1, FDC1004Channel::CIN1);
}

void loop() {
  fdc.triggerSingleMeasurement(FDC1004Measurement::M1);

  if (fdc.waitForMeasurement(FDC1004Measurement::M1, 100) == FDCxStatus::Ok) {
    float pf = 0.0f;

    if (fdc.readLastCapacitancePf(FDC1004Measurement::M1, pf) == FDCxStatus::Ok) {
      Serial.print("Capacitance: ");
      Serial.print(pf, 6);
      Serial.println(" pF");
    }
  }

  delay(250);
}
```

---

## Quick Start — FDC2x1x Basic Frequency

```cpp
#include <Wire.h>
#include <FDCxCapSense_FDC2x1x.h>

FDC2x1x fdc;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  FDCxStatus status = fdc.begin(Wire, FDC2x1xVariant::FDC2214, 0x2A);
  if (status != FDCxStatus::Ok) {
    Serial.println("FDC2x1x device not detected");
    return;
  }

  // These are example configuration values. Real hardware values must be selected
  // from the datasheet based on the LC tank, reference clock and sensor topology.
  fdc.setSleepMode(true);
  fdc.setRCount(FDC2x1xChannel::CH0, 0x0100);
  fdc.setSettleCount(FDC2x1xChannel::CH0, 0x0100);
  fdc.configureClockDividers(FDC2x1xChannel::CH0, 2, 1);
  fdc.setDriveCurrentCode(FDC2x1xChannel::CH0, 15);
  fdc.setDeglitch(FDC2x1xDeglitch::MHz_10);
  fdc.startContinuous(FDC2x1xChannel::CH0);
}

void loop() {
  FDC2x1xFrequency reading;

  if (fdc.waitDataReady(FDC2x1xChannel::CH0, 100) == FDCxStatus::Ok &&
      fdc.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, reading) == FDCxStatus::Ok) {
    Serial.print("Sensor frequency: ");
    Serial.print(reading.frequencyHz, 2);
    Serial.println(" Hz");
  }

  delay(250);
}
```

`configureChannel()` is retained as a validation-only compatibility alias in v0.6.x. It does **not** fully configure the resonator or start conversion. Real FDC2x1x hardware setup requires datasheet-based `setRCount()`, `setSettleCount()`, `configureClockDividers()` / `writeRawClockDividers()`, `setDriveCurrentCode()`, `setDeglitch()`, and then `startContinuous()` to clear `CONFIG.SLEEP_MODE_EN`. Prefer `readFrequencyHzFromRegisters()` for real hardware because it decodes `CLOCK_DIVIDERS_CHx` from the device registers.

---

## Architecture: FDC1004 vs FDC2x1x

| Feature | FDC1004 | FDC2x1x |
|---|---|---|
| Physical model | Direct capacitance-to-digital converter | Resonant LC / frequency-based sensing |
| Primary output | Capacitance-like direct measurement | Frequency/raw code |
| Capacitance | Direct pF path | Derived from frequency and LC parameters |
| Calibration | CAPDAC, baseline, temperature/reference compensation | LC parameters, baseline frequency, reference clock |
| Profile use | Direct capacitance sample | Frequency or derived sample with metadata |

FDCxCapSense uses metadata-aware samples to avoid mixing these two measurement models.

For FDC2x1x derived capacitance, `FDC2x1xLCParams.traceReferenceClockHz` is trace metadata only. The derivation helper uses already-converted `frequencyHz`, `inductanceUH`, and `parasiticCapacitancePf`. The effective reference clock belongs to the `readFrequencyHzFromRegisters()` or explicit `readFrequencyHz(..., frefHz, finSelMultiplier, ...)` step.

---

## Experimental profiles

The library includes experimental helpers for:

- liquid level
- proximity
- surface-state candidates
- material feature extraction
- powder/granular proxy sensing
- automotive proximity demo

These profiles provide:

- state
- confidence
- reason
- raw metrics
- diagnostic flags

They do **not** provide certified, guaranteed, or safety-critical decisions.

Official profile examples use metadata-aware inputs through `FDCxSample` and `FDCxFeatureVector`. Raw-float profile helpers are compatibility-only; define `FDCX_DISABLE_LEGACY_PROFILE_HELPERS` before including the library if you want to hide those overloads from your build.

---

## Important limitations

FDCxCapSense does **not** provide:

- certified safety decisions
- not automotive-grade validation
- field validation by the Developer
- guaranteed black ice detection
- guaranteed liquid/material classification
- hardware validation by the Developer

Application profiles are experimental and calibration-dependent. Real-world deployment requires electrode design, shielding, grounding, cable length review, environment testing, and owner-side hardware validation.

Allowed wording:

- experimental
- calibration-aware
- research-first
- hardware validation required
- owner-side validation required
- derived capacitance is model-dependent

---

## Project positioning

FDCxCapSense is intended as a reference-style Arduino library for developers working on:

- capacitive sensing
- industrial IoT sensing
- embedded sensor research
- liquid level sensing
- proximity sensing
- material/surface response experiments
- sensor calibration workflows
- edge/field prototype development

This project also reflects the engineering approach behind Proio: building practical embedded and industrial IoT systems from low-level sensor behavior to calibration, diagnostics, and application-level interpretation.

---

## GitHub discovery metadata

Suggested repository description:

```text
Arduino library for TI FDC1004, FDC1004-Q1 and FDC2x1x/FDC2214 capacitive sensing with calibration, CAPDAC, diagnostics, liquid level, proximity and surface-state profiles.
```

Suggested GitHub topics:

```text
arduino arduino-library capacitive-sensing fdc1004 fdc2214 fdc2x1x i2c esp32 liquid-level-sensing proximity-sensing fdc1004-q1 fdc2212 fdc2114 fdc2112 sensor-calibration industrial-iot embedded
```

Optional additional topics:

```text
capacitance-sensor capacitance-to-digital touch-sensing surface-sensing sensor-fusion embedded-systems edge-ai
```

---

## Feedback and collaboration

This library is free for the community.

If you test it with real hardware, improve documentation, find a bug, notice a datasheet mismatch, or want to suggest a better workflow, please open an issue.

We are especially interested in feedback from:

- Arduino users
- embedded developers
- industrial IoT engineers
- sensor researchers
- FDC1004/FDC2214 users
- teams building capacitive sensing products or prototypes

Maintainer: **Parvaz Jamei**  
LinkedIn: [linkedin.com/in/parvaz-jamei](https://www.linkedin.com/in/parvaz-jamei)  
Website: [proio.ir](https://proio.ir)

---

## Test and CI status

Bundled checks:

```bash
tools/clean_package_check.sh
tools/claim_safety_scan.sh
scripts/architecture_contract_scan.sh
tools/host_smoke_compile.sh
scripts/compile_examples_arduino_cli.sh arduino:avr:uno
scripts/compile_examples_arduino_cli.sh arduino:samd:mkrzero
scripts/compile_examples_arduino_cli.sh esp32:esp32:esp32
```

If Arduino CLI is not installed, Arduino compile is reported as `NOT RUN` instead of a false PASS. GitHub Actions workflows are included for owner-side CI execution, including compile examples, unit-test smoke checks, and Arduino Lint.

---

## Hardware validation status

Hardware validation was **not performed by the Developer**. Hardware testing is owner-side and must be recorded in `docs/hardware_validation_matrix.md` before any field or product interpretation.

---

## Clean-room and license policy

MIT License.

This project follows a strict clean-room policy. Existing libraries may be inspected for research, risk discovery, learning, and conceptual comparison. Third-party code, examples, tests, documentation text, API shape, file layout, naming pattern, or recognizable implementation structure must not be copied or closely derived without explicit license review, compatibility check, approval, and attribution.

Official implementation source of truth:

- TI official datasheets/application notes
- Arduino official documentation
- independently written project code

---

## Status

Current status:

```text
Public Release Candidate
Code/package-level accepted for owner-side handoff
Hardware validation: owner-side / not performed by Developer
Arduino Library Manager submission: pending public repo URL, CI, lint and tag
```

Owner-side release blockers before public submission:

- Repository URL is set to `https://github.com/Parvaz-Jamei/FDCxCapSense`; owner must confirm the public repository exists before tagging/submission.
- Run GitHub Actions compile matrix and Arduino Lint on the public repository.
- Record owner-side hardware evidence in `docs/hardware_validation_matrix.md`.
- Check Arduino Library Manager name uniqueness for `FDCxCapSense`.
- Create a SemVer tag only after metadata, CI, and documentation are final.

See:

- `docs/library_manager_checklist.md`
- `docs/release_process.md`
- `docs/maintenance_plan.md`
- `docs/release_candidate_checklist.md`


## v0.6.7 FDC2x1x real-hardware startup fixes

- FDC2x1x `setDeglitch()` now preserves the datasheet/default `MUX_CONFIG` reserved pattern `0x0208` while updating only deglitch bits and preserving autoscan/sequence bits.
- Added safe startup APIs: `setSleepMode()`, `setActiveChannel()`, `startContinuous()`, `stopConversions()`, and `configureSingleChannel()`.
- Public FDC2x1x examples now explicitly configure channel timing/current/deglitch, then start continuous conversion before waiting for data.
- Safe `setRCount()` rejects reserved low public values below `0x0100`; `writeRawRCount()` remains an explicit raw path.
- `readFrequencyHzFromRegisters()` remains the recommended hardware path because it decodes the clock divider register instead of relying on a stale caller-side assumption.

## v0.6.7 publish-blocker fixes

This package tightens the FDC2x1x publish path: `waitDataReady()` is channel-aware, DATA MSB error flags are rejected before raw masking, FDC221x frequency conversion can decode `CHx_FIN_SEL` and `CHx_FREF_DIVIDER` from `CLOCK_DIVIDERS_CHx`, and FDC1004 readings now report the configured CIN channel. Arduino CLI compile remains owner-side if the tool is unavailable in the Developer environment.


## v0.6.7 FDC2x1x CONFIG register fixes

- `CONFIG` raw writes now validate the writable reserved-bit pattern `0x1401` instead of treating real option bits as fixed reserved bits.
- Safe helpers preserve `SENSOR_ACTIVATE_SEL`, `REF_CLK_SRC`, `INTB_DIS`, and `HIGH_CURRENT_DRV` while changing sleep/active-channel state.
- `setReferenceClockSource()` makes external-clock sketches explicit; public examples set external clock before using `40000000.0f`.
- `configureSingleChannel()` now clears `AUTOSCAN_EN` / `RR_SEQUENCE` and writes a single-channel `MUX_CONFIG` value.
- Two-channel variants reject raw `CONFIG` active channels and `MUX_CONFIG` autoscan sequences that include unavailable CH2/CH3.
- Single-ended topology helpers force the documented `FIN_SEL` path used by examples.

FDC211x remains raw/foundation-only for v0.6.x precise frequency conversion; FDC221x remains the supported frequency-conversion path. Hardware validation is still owner-side and not claimed by this package.



## v0.6.11 industrial release hardening

- `ERROR_CONFIG` raw writes now reject reserved bits and accept only documented alert/interrupt routing bits.
- High-level FDC2x1x configuration setters require Sleep Mode, matching the datasheet-oriented `sleep -> configure -> startContinuous()` flow. Raw register writers remain explicit advanced APIs.
- `FDC2x1xDerivedCapacitance::toSample()` clamps NaN confidence to `0.0f` and still clamps negative/above-one values to `[0, 1]`.
- The legacy `readFrequencyHz(channel, frefHz, output)` overload is strongly marked as a compatibility helper and now reads `CHx_FIN_SEL` before calculating instead of silently assuming `FIN_SEL=1`. Real hardware examples still prefer `readFrequencyHzFromRegisters()`.
- FDC2112/FDC2114 remain raw/foundation-only for v0.6.x; precise FDC221x frequency conversion is the supported public conversion path.
- FDC1004 `readCapacitancePf()` / `readLastCapacitancePf()` are latest-register reads; use `readSingleMeasurement()` for fresh single-shot conversion.

## v0.6.8 FDC2x1x release edge-case fixes

- Two-channel FDC2112/FDC2212 autoscan validation now accepts `RR_SEQUENCE=b00` and `b11` for CH0/CH1, and still rejects sequences that include CH2/CH3.
- `RESET_DEV` raw writes reject reserved bits; FDC221x rejects FDC211x-only `OUTPUT_GAIN` bits.
- `CONFIG.HIGH_CURRENT_DRV` is guarded as CH0 single-channel only and cannot coexist with `MUX_CONFIG.AUTOSCAN_EN`.
- Differential clock-divider setup now has `configureClockDividersForDifferential(channel, finSelMultiplier, frefDivider)` so `FIN_SEL=1` or `FIN_SEL=2` is explicit.
- Official FDC2x1x examples keep using `readFrequencyHzFromRegisters()`; the simpler `readFrequencyHz(channel, frefHz, output)` overload is legacy compatibility only.
- FDC1004 `readCapacitancePf()` is documented as latest-register read; use `readSingleMeasurement()` for a fresh single-shot conversion.
