# Research Ledger

Project rule: search, study, record source, review license, then code. This ledger records official references and inspected third-party libraries. Third-party libraries are inspection/research-only unless an explicit future reuse decision is documented.

## 1. Historical internal roadmap source

- Name: FDCxCapSense 5-Phase Reference Library Roadmap v4 (historical reference)
- URL: Local uploaded source file: `FDCxCapSense_5_phase_reference_roadmap_v4.html`
- Type: Project roadmap / internal planning source
- License: Project-owned/internal reference
- Used as: Reference only
- License impact: Project-owned/internal; no third-party reuse concern.
- Notes: Defines clean-room, license-safe, calibration-first direction; excludes application profiles and FDC2x1x implementation from early phases.

## 2. TI FDC1004 product page

- Name: Texas Instruments FDC1004 product page
- URL: https://www.ti.com/product/FDC1004
- Type: Official datasheet / official product docs
- License: TI official documentation / website terms
- Used as: Reference only
- License impact: Official reference; no long-text copying.
- Notes: Product page identifies the FDC1004 as a 4-channel capacitance-to-digital converter with active shield driver, I2C, 100/200/400 S/s output rates, and capacitive sensing applications.

## 3. TI FDC1004 datasheet Rev. C

- Name: FDC1004 4-Channel Capacitance-to-Digital Converter for Capacitive Sensing Applications datasheet Rev. C
- URL: https://www.ti.com/lit/ds/symlink/fdc1004.pdf
- Type: Official datasheet
- License: TI official documentation / website terms
- Used as: Primary register/timing/measurement reference
- License impact: Official reference; no long-text copying.
- Notes: Primary source for I2C address, Manufacturer ID `0x5449`, Device ID `0x1004`, measurement registers, FDC config fields, CAPDAC encoding, output rates, ±15 pF range, offset handling, and conversion formula.

## 4. TI FDC1004-Q1 product page

- Name: Texas Instruments FDC1004-Q1 product page
- URL: https://www.ti.com/product/FDC1004-Q1
- Type: Official product docs
- License: TI official documentation / website terms
- Used as: Reference only
- License impact: Official reference; no long-text copying.
- Notes: Automotive variant reference. No automotive claim is made by this library.

## 5. TI FDC1004-Q1 datasheet

- Name: FDC1004-Q1 4-Channel Capacitance-to-Digital Converter for Capacitive Sensing Applications datasheet
- URL: https://www.ti.com/lit/ds/symlink/fdc1004-q1.pdf
- Type: Official datasheet
- License: TI official documentation / website terms
- Used as: Reference only
- License impact: Official reference; no long-text copying.
- Notes: Used to compare automotive variant boundaries only. No Q1-specific validation claim is made.

## 6. TI FDC2x1x / FDC2214 datasheet

- Name: FDC2112/FDC2114/FDC2212/FDC2214 datasheet
- URL: https://www.ti.com/lit/ds/symlink/fdc2214.pdf
- Type: Official datasheet
- License: TI official documentation / website terms
- Used as: Architecture separation reference
- License impact: Official reference; no long-text copying.
- Notes: Confirms FDC2x1x uses a resonant/LC sensing architecture and must not be mixed into the FDC1004 driver path.

## 7. Arduino Library Specification

- Name: Arduino CLI Library Specification
- URL: https://docs.arduino.cc/arduino-cli/library-specification/
- Type: Official docs
- License: Arduino documentation terms
- Used as: Library structure and metadata reference
- License impact: Official reference; no long-text copying.
- Notes: Used for `library.properties`, `src/` structure, public include behavior, examples, and metadata.

## 8. AUnit documentation

- Name: AUnit Arduino library documentation
- URL: https://docs.arduino.cc/libraries/aunit/
- Type: Official Arduino library docs / test framework reference
- License: AUnit project license and Arduino docs terms
- Used as: Test framework reference only
- License impact: Dependency/reference only; no source reuse.
- Notes: Used for AUnit-compatible test sketch structure. AUnit is not bundled in the library.

## Phase 2 source recheck

### 9. FDC1004: Basics of Capacitive Sensing and Applications

- Name: FDC1004: Basics of Capacitive Sensing and Applications
- URL: https://www.ti.com/lit/an/snoa927a/snoa927a.pdf
- Type: Official TI application note
- License: TI official documentation / website terms
- Used as: Conceptual capacitive-sensing reference
- License impact: Official reference; no long-text copying.
- Notes: Used to keep Phase 2 focused on generic sensing infrastructure and environmental considerations, not product classifiers.

### 10. How to Calibrate FDC1004 for Liquid Level Sensing Applications

- Name: How to Calibrate FDC1004 for Liquid Level Sensing Applications
- URL: https://www.ti.com/lit/pdf/snoa958
- Type: Official TI application report
- License: TI official documentation / website terms
- Used as: Calibration concepts reference only
- License impact: Official reference; no long-text copying.
- Notes: Used to justify generic baseline/correction infrastructure. Phase 2 did not implement a liquid-level profile; Phase 3 now includes an experimental, metadata-aware liquid-level profile with no liquid-level product claim.

### 11. Capacitive Sensing: Ins and Outs of Active Shielding

- Name: Capacitive Sensing: Ins and Outs of Active Shielding
- URL: https://www.ti.com/lit/an/snoa940/snoa940.pdf
- Type: Official TI application note
- License: TI official documentation / website terms
- Used as: Shielding behavior reference only
- License impact: Official reference; no long-text copying.
- Notes: Used to document why CAPDAC disabled/enabled behavior should be explicit and tested.

## Third-party libraries inspected for research only

These libraries may be inspected for risk discovery, API comparison, behavior comparison, and conceptual learning. No source reuse, close rewrite, example/test reuse, API-shape reuse, file-layout reuse, naming-pattern reuse, or recognizable implementation derivation is permitted without separate documented approval.

### 12. ProtoCentral FDC1004 library

- Name: ProtoCentral FDC1004 Arduino library / examples
- URL: https://github.com/Protocentral/ProtoCentral_fdc1004_breakout
- Type: Existing library / example
- License: Check repository before any reuse
- Used as: Inspected for research only; no code/text/test/example/API/source-structure reuse.
- License impact: Strict; reuse forbidden unless separately reviewed and approved.
- Notes: Do not copy code, examples, comments, register tables from source files, API shape, or implementation logic.

### 13. slight_FDC1004

- Name: slight_FDC1004
- URL: https://github.com/search?q=slight_FDC1004&type=repositories
- Type: Existing library / reference candidate
- License: Check repository before any reuse
- Used as: Inspected for research only; no code/text/test/example/API/source-structure reuse.
- License impact: Strict; reuse forbidden unless separately reviewed and approved.
- Notes: Research-only.

### 14. beshaya/FDC1004

- Name: beshaya/FDC1004
- URL: https://github.com/beshaya/FDC1004
- Type: Existing library / reference candidate
- License: Check repository before any reuse
- Used as: Inspected for research only; no code/text/test/example/API/source-structure reuse.
- License impact: Strict; reuse forbidden unless separately reviewed and approved.
- Notes: Research-only.

### 15. zharijs/FDC2214

- Name: zharijs/FDC2214
- URL: https://github.com/zharijs/FDC2214
- Type: Existing library / FDC2x1x reference candidate
- License: Check repository before any reuse
- Used as: Inspected for research only; no code/text/test/example/API/source-structure reuse.
- License impact: Strict; reuse forbidden unless separately reviewed and approved.
- Notes: FDC2x1x frequency-first foundation is added in v0.6.5 Strict Sensor Correctness Patch with separate resonant-LC physics; Phase 3 application profiles still reject FDC2x1x derived samples unless an explicit future-compatible profile path is added.

### 16. dac1e/FDC2x1x

- Name: dac1e/FDC2x1x
- URL: https://github.com/dac1e/FDC2x1x
- Type: Existing library / FDC2x1x reference candidate
- License: Check repository before any reuse
- Used as: Inspected for research only; no code/text/test/example/API/source-structure reuse.
- License impact: Strict; reuse forbidden unless separately reviewed and approved.
- Notes: FDC2x1x frequency-first foundation is added in v0.6.5 Strict Sensor Correctness Patch with separate resonant-LC physics; Phase 3 application profiles still reject FDC2x1x derived samples unless an explicit future-compatible profile path is added.

### 17. Zephyr FDC drivers

- Name: Zephyr sensor drivers related to FDC devices
- URL: https://github.com/zephyrproject-rtos/zephyr
- Type: Existing driver framework / RTOS source
- License: Apache-2.0 for Zephyr project, but verify exact file before any reuse
- Used as: Inspected for research only; no code/text/test/example/API/source-structure reuse.
- License impact: Strict; reuse forbidden unless separately reviewed and approved.
- Notes: No Zephyr code or file structure is reused.


## Phase 2.1 / Phase 3 source recheck

- Name: TI FDC1004 datasheet Rev. C
  URL: https://www.ti.com/lit/ds/symlink/fdc1004.pdf
  Type: Official datasheet
  License: TI documentation; reference only
  Used as: Register, CAPDAC, measurement, I2C, sample-rate, and FDC configuration source of truth. No text/table/code reuse.
  Notes: Power/idle API is conservative: it clears REPEAT and trigger bits only. No current-consumption claim is made.

- Name: TI FDC1004 product page
  URL: https://www.ti.com/product/FDC1004
  Type: Official product documentation
  License: TI documentation; reference only
  Used as: High-level application context and capability boundary. No product claims copied.
  Notes: Application-profile examples remain experimental and calibration-aware.

- Name: Arduino Library Specification
  URL: https://docs.arduino.cc/arduino-cli/library-specification/
  Type: Official docs
  License: Documentation; reference only
  Used as: Project layout and public include behavior reference.
  Notes: `src/` and subfolders contain library sources; only `src/` enters the sketch include path.

- Name: AUnit documentation
  URL: https://docs.arduino.cc/libraries/aunit/
  Type: Official library docs
  License: Reference only
  Used as: Optional Arduino-compatible test framework reference.
  Notes: Tests are written to stay simple and portable; hardware testing is not claimed.

## Roadmap v6 alignment source recheck

- Name: FDCxCapSense Roadmap v6 Implementation-Safe
  - URL: local project planning file `FDCxCapSense_5_phase_reference_roadmap_v6_implementation_safe.html`
  - Type: Project roadmap / implementation-safety requirements
  - License: Project-owned planning document
  - Used as: Architecture and scope authority for v0.6.5 Phase 5 owner release handoff
  - Notes: Defines the Roadmap v6 principle: unified brand, separated physics, separated implementation. It requires metadata-aware profile inputs, header-budget control, and no FDC2x1x implementation before Phase 4.

- Name: TI FDC1004 official product page
  - URL: https://www.ti.com/product/FDC1004
  - Type: Official product page / datasheet entry
  - License: Official vendor documentation, reference only
  - Used as: Reference only
  - Notes: TI describes FDC1004 as a high-resolution 4-channel capacitance-to-digital converter with full-scale range about +/-15 pF and offset capacitance handling up to 100 pF.

- Name: TI FDC1004 datasheet Rev. C
  - URL: https://www.ti.com/lit/ds/symlink/fdc1004.pdf
  - Type: Official datasheet
  - License: Official vendor documentation, reference only
  - Used as: Reference only
  - Notes: Source of truth for FDC1004 register behavior, direct capacitance measurement model, CAPDAC, sample rates, and active shield notes.

- Name: TI FDC1004-Q1 official product/datasheet page
  - URL: https://www.ti.com/product/FDC1004-Q1
  - Type: Official automotive-qualified product page / datasheet entry
  - License: Official vendor documentation, reference only
  - Used as: Reference only
  - Notes: Used to document Q1 vs non-Q1 positioning. It does not create an automotive-grade library claim.

- Name: TI FDC2214 / FDC2x1x official product/datasheet
  - URL: https://www.ti.com/product/FDC2214 and https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  - Type: Official product page / datasheet
  - License: Official vendor documentation, reference only
  - Used as: Official reference for v0.6.5 Phase 4 FDC2x1x frequency-first foundation
  - Notes: The FDC2x1x family uses resonant/narrow-band LC sensing and must remain physically and architecturally separated from FDC1004 direct-CDC implementation.

- Name: Arduino Library Specification
  - URL: https://docs.arduino.cc/arduino-cli/library-specification/
  - Type: Official docs
  - License: Official documentation, reference only
  - Used as: Reference only
  - Notes: Source of truth for `library.properties`, `includes`, `src/` layout, root header interface behavior, and recursive source compilation.

- Name: GitHub Docs — Licensing a repository
  - URL: https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/licensing-a-repository
  - Type: Official docs
  - License: Official documentation, reference only
  - Used as: Reference only
  - Notes: Confirms that without a license, default copyright applies and others may not reproduce, distribute, or create derivative works.


## Phase 4 source recheck

- Name: TI FDC2214 / FDC2x1x official product and datasheet
  URL: https://www.ti.com/product/FDC2214 ; https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  Type: Official datasheet / official product page
  License: TI documentation; reference only
  Used as: Official implementation source for v0.6.5 Phase 4 FDC2x1x frequency-first architecture
  Notes: FDC2x1x is resonant/narrow-band sensing. It is kept separate from FDC1004 direct CDC.

- Name: Arduino Library Specification
  URL: https://docs.arduino.cc/arduino-cli/library-specification/
  Type: Official docs
  License: Documentation reference
  Used as: Arduino metadata and includes/header-budget reference

- Name: GitHub licensing documentation
  URL: https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/licensing-a-repository
  Type: Official docs
  License: Documentation reference
  Used as: Strict license policy reference

- Name: Public FDC2x1x libraries / Zephyr driver
  URL: Research-only inspection sources
  Type: Existing library / driver reference
  License: Strict; not reused
  Used as: Research-only inspection; no code/text/test/example/API/file-layout reuse
  Notes: No source-level reuse is allowed unless separately reviewed and approved.

## Phase 4 v0.6.5 accuracy-fix source recheck

- Name: TI FDC2x1x datasheet Rev. B
  URL: https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  Type: Official datasheet
  License: Official vendor documentation; reference only
  Used as: Source of truth for v0.6.5 FDC2x1x identity checks, 12-bit vs 28-bit raw behavior, resonant sensing model, and register policy
  Notes: Manufacturer ID is documented as `0x5449`; device ID is `0x3054` for FDC211x and `0x3055` for FDC221x. FDC211x is treated as 12-bit raw/foundation-only in v0.6.5; FDC221x is treated as 28-bit frequency-first.

- Name: Arduino Library Specification
  URL: https://docs.arduino.cc/arduino-cli/library-specification/
  Type: Official docs
  License: Documentation reference
  Used as: Source of truth for `src/` recursive compilation and `library.properties` includes/header budget.

## Phase 5 owner release handoff source recheck

- Name: Arduino Library Specification
  URL: https://docs.arduino.cc/arduino-cli/library-specification/
  Type: Official docs
  License: Documentation reference only
  Used as: Official source for `library.properties`, `src/`, examples, and `includes` behavior
  Notes: Only `src/` is added to the include path; the `includes` field controls headers suggested by Arduino IDE.

- Name: Arduino Library Manager submission guide
  URL: https://support.arduino.cc/hc/en-us/articles/360012175419-How-to-submit-a-third-party-library-to-the-Arduino-Library-Manager
  Type: Official docs
  License: Documentation reference only
  Used as: Official release-readiness reference
  Notes: Submission is owner-side and was not performed by the Developer.

- Name: arduino/setup-arduino-cli
  URL: https://github.com/arduino/setup-arduino-cli
  Type: Official GitHub Action
  License: MIT
  Used as: CI setup reference only
  Notes: Workflow uses this action to make Arduino CLI available in GitHub Actions.

- Name: TI FDC2214 product/datasheet
  URL: https://www.ti.com/product/FDC2214
  Type: Official datasheet/product page
  License: Documentation reference only
  Used as: Official implementation source for FDC2x1x frequency-first architecture
  Notes: Confirms resonant sensing architecture; implementation remains clean-room.

- Name: GitHub Licensing documentation
  URL: https://docs.github.com/en/repositories/managing-your-repositorys-settings-and-features/customizing-your-repository/licensing-a-repository
  Type: Official docs
  License: Documentation reference only
  Used as: License-policy reference
  Notes: Reinforces strict clean-room and no-reuse policy for third-party libraries unless reviewed and approved.

## v0.6.5 publish-blocker source recheck

- TI FDC2x1x / FDC2214 official datasheet
  - Type: Official datasheet
  - License: TI documentation; reference only
  - Used as: Source of truth for LC resonator architecture, FDC211x/FDC221x separation, DATA MSB error handling, STATUS channel-ready flags, and CLOCK_DIVIDERS_CHx behavior.
  - Notes: FDC221x frequency conversion uses DATAx with effective fREF and CHx_FIN_SEL multiplier. FDC211x precise conversion remains `NotSupported` in v0.6.x.

- Arduino CLI documentation
  - Type: Official docs
  - License: Official documentation; reference only
  - Used as: Source of truth for release-mode example compilation behavior.

- Arduino Library Specification
  - Type: Official docs
  - License: Official documentation; reference only
  - Used as: Source of truth for `src/`, `library.properties`, and `includes` behavior.

## v0.6.5 strict sensor correctness source recheck

- Name: TI FDC2214 / FDC2x1x official datasheet
  URL: https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  Type: Official datasheet
  License: TI documentation; reference only
  Used as: Official implementation source for FDC2x1x resonant/frequency-first behavior, channel-specific unread-conversion status, DATA MSB error flags, `CLOCK_DIVIDERS_CHx`, `CHx_FIN_SEL`, and `CHx_FREF_DIVIDER`.
  Notes: FDC2x1x remains separate from FDC1004 direct CDC. Existing public drivers/libraries were not reused.

- Name: Arduino CLI compile documentation
  URL: https://arduino.github.io/arduino-cli/dev/commands/arduino-cli_compile/
  Type: Official docs
  License: Official Arduino documentation; reference only
  Used as: Reference for strict release-gate behavior of `scripts/compile_examples_arduino_cli.sh`.
  Notes: Missing `arduino-cli` must not be treated as PASS in release mode; local skips require `ALLOW_NO_ARDUINO_CLI=1` and must be recorded as NOT RUN.

- Name: Arduino Library Specification
  URL: https://docs.arduino.cc/arduino-cli/library-specification/
  Type: Official docs
  License: Official Arduino documentation; reference only
  Used as: Reference for source layout, `src/` include path behavior, and controlled `includes` metadata.
  Notes: Public root headers remain intentional and controlled.

## v0.6.7 FDC2x1x real-hardware startup recheck

- Name: TI FDC2112/FDC2114/FDC2212/FDC2214 datasheet
  URL: https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  Type: Official datasheet
  License: TI official documentation / website terms
  Used as: Primary CONFIG, MUX_CONFIG, Sleep Mode, ACTIVE_CHAN, RCOUNT, SETTLECOUNT, CLOCK_DIVIDERS, DRIVE_CURRENT, STATUS and DATA_CHx register reference.
  Notes: Used to correct startup flow so configuration happens while asleep, then conversion is started by clearing CONFIG.SLEEP_MODE_EN. Register constants were implemented clean-room from the datasheet and prior project QA notes.

- Name: TI FDC2214 Proximity and Capacitive Touch EVM User's Guide
  URL: https://www.ti.com/lit/pdf/snou139
  Type: Official TI EVM / configuration reference
  License: TI official documentation / website terms
  Used as: Configuration sanity-check only.
  Notes: No EVM code or text was reused.

- Name: zharijs/FDC2214 Arduino library
  URL: https://github.com/zharijs/FDC2214
  Type: Existing Arduino library
  License: Check repository before any reuse
  Used as: Research-only sanity-check for the conceptual MUX_CONFIG pattern `0x0208 | autoscan | deglitch`.
  Notes: No code, comments, tests, examples, file layout, API shape or documentation text was copied or closely rewritten.

- Name: Zephyr FDC2x1x sensor driver and sample documentation
  URL: https://docs.zephyrproject.org/latest/doxygen/html/group__fdc2x1x__interface.html
  Type: Existing driver/API documentation
  License: Zephyr project licensing; verify exact source before any reuse
  Used as: Research-only sanity-check for supported FDC2112/FDC2114/FDC2212/FDC2214 family behavior.
  Notes: No Zephyr code, tests, examples, file layout, API shape or documentation text was copied or closely rewritten.

- Name: TI FDC1004 datasheet Rev. C
  URL: https://www.ti.com/lit/ds/symlink/fdc1004.pdf
  Type: Official datasheet
  License: TI official documentation / website terms
  Used as: Rechecked FDC_CONF, REPEAT, single-shot measurement flow, measurement configuration, and OFFSET_CAL_CINx / GAIN_CAL_CINx boundaries.
  Notes: High-level API remains single-ended; low-level calibration register support is documented as not part of the high-level calibration API yet.


## v0.6.7 FDC2x1x CONFIG register recheck

- Name: TI FDC2112/FDC2114/FDC2212/FDC2214 datasheet
  URL: https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  Type: Official datasheet
  License: TI official documentation / website terms
  Used as: Primary reference for `CONFIG`, `REF_CLK_SRC`, `SENSOR_ACTIVATE_SEL`, `ACTIVE_CHAN`, `SLEEP_MODE_EN`, `MUX_CONFIG.AUTOSCAN_EN`, `RR_SEQUENCE`, and reset/default register behavior.
  Notes: Used for clean-room register validation only; no source code or documentation text was copied.

- Name: Zephyr FDC2x1x binding/driver documentation
  URL: https://docs.zephyrproject.org/latest/build/dts/api/bindings/sensor/ti%2Cfdc2x1x.html
  Type: Existing driver documentation
  License: Zephyr project licensing; verify exact source before any reuse
  Used as: Research-only sanity-check for family/variant coverage.
  Notes: No code, test, API shape or documentation text was reused.

## v0.6.8 FDC2x1x release edge-case recheck

- Name: TI FDC2112/FDC2114/FDC2212/FDC2214 datasheet
  URL: https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  Type: Official datasheet
  License: TI official documentation / website terms
  Used as: Primary reference for `MUX_CONFIG.RR_SEQUENCE`, `RESET_DEV.RESET_DEV`, `RESET_DEV.OUTPUT_GAIN`, `CONFIG.HIGH_CURRENT_DRV`, and high-current single-channel limitations.
  Notes: Used for clean-room register validation and edge-case hardening only; no source code or documentation text was copied.

## v0.6.10 industrial release hardening recheck

- Name: TI FDC2112/FDC2114/FDC2212/FDC2214 datasheet
  URL: https://www.ti.com/lit/ds/symlink/fdc2214.pdf
  Type: Official datasheet
  License: TI official documentation / website terms
  Used as: Primary reference for `ERROR_CONFIG`, Sleep Mode configuration discipline, `CLOCK_DIVIDERS_CHx.FIN_SEL`, FDC211x/FDC221x output-width separation, and resonant/frequency-first claim boundaries.
  Notes: Used for clean-room validation and release hardening only; no source code or documentation text was copied.

- Name: Zephyr FDC2x1x sensor driver/binding documentation
  URL: https://docs.zephyrproject.org/latest/build/dts/api/bindings/sensor/ti%2Cfdc2x1x.html
  Type: Existing driver documentation
  License: Zephyr project licensing; verify exact source before any reuse
  Used as: Research-only sanity-check for FDC2112/FDC2114/FDC2212/FDC2214 family coverage and variant-aware separation.
  Notes: No Zephyr code, tests, examples, API shape or documentation text was copied or closely rewritten.


## v0.6.11 final industrial quality polish recheck

Research-only sources rechecked before the final industrial quality polish patch:

- Texas Instruments FDC2x1x/FDC2214 datasheet: Sleep Mode register access and conversion start by clearing CONFIG.SLEEP_MODE_EN.
- Texas Instruments FDC2x1x/FDC2214 datasheet: HIGH_CURRENT_DRV is a CH0 single-channel mode option and must not be combined with autoscan/multi-channel behavior.
- Arduino Library Specification: `library.properties` must be at the library root and contains metadata fields such as `url`, `architectures`, and `includes`.

No third-party code, comments, tests, API shape, or documentation text was copied or adapted from external libraries.
