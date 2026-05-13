# Public Release Notes - v0.6.11 Final Industrial Quality Polish

FDCxCapSense v0.6.11 is a final pre-owner-gate hardening package. It closes ERROR_CONFIG reserved-bit validation, active-mode high-level configuration guards, NaN confidence sanitization, legacy readFrequencyHz footgun reduction, FDC211x scope clarity, and FDC1004 latest-register read semantics. Hardware validation and CI evidence remain owner-side release gates; the repository URL is set and must be confirmed public before tagging.

## v0.6.8 FDC2x1x release edge-case fix

This package is a release-edgecase hardening patch. It does not add new sensing profiles or claim hardware validation.


## v0.6.8 FDC2x1x publish-blocker fix

- Decode FDC221x frequency from `CLOCK_DIVIDERS_CHx` using `CHx_FIN_SEL` and `CHx_FREF_DIVIDER`.
- Make `waitDataReady()` channel-aware and reject STATUS error bits.
- Reject DATA MSB watchdog/amplitude warning flags before raw masking.
- Add safer FDC2x1x clock-divider helper and raw register writer naming.
- Make FDC1004 readings report the actual configured CIN channel and reject unconfigured measurement reads.
- Harden profile edge-case validation and release compile script behavior.

# Public Release Notes - v0.6.8 README / Strict Sensor Correctness Patch

FDCxCapSense v0.6.8 is a public release candidate package focused on Arduino Library Manager searchability, GitHub discoverability, and owner-side release handoff.

## Positioning

FDCxCapSense is an Arduino library for TI FDC1004, FDC1004-Q1, FDC2112, FDC2114, FDC2212, and FDC2214 capacitive sensing ICs.

It is not just a breakout driver. It is a calibration-first, family-aware Arduino reference library for TI FDC capacitive sensing workflows.

## Discovery / SEO updates

- `library.properties` now uses a more search-friendly `sentence` and `paragraph`.
- The README now explicitly mentions TI FDC1004, FDC1004-Q1, FDC2x1x, FDC2112, FDC2114, FDC2212, FDC2214, CAPDAC, diagnostics, calibration, liquid level, proximity, material response, surface-state candidates, ESP32, and industrial IoT contexts.
- GitHub repository description and topic suggestions are documented in `docs/release_process.md`.
- The maintainer LinkedIn link is included in the README for community context: https://www.linkedin.com/in/parvaz-jamei

## Known limitations

- Hardware validation is owner-side and was not performed by the Developer.
- FDC211x precise frequency conversion is not implemented in v0.6.x.
- Application profiles are experimental candidate helpers only.
- Derived capacitance is model-dependent.
- Arduino Library Manager submission remains blocked until the configured public repository exists, CI/lint pass, and a SemVer tag is created.

## Owner-side release blockers

- Repository URL is set to `https://github.com/Parvaz-Jamei/FDCxCapSense`; owner must confirm the public repository exists before tagging/submission.
- Run GitHub Actions compile matrix.
- Run Arduino Lint.
- Check Arduino Library Manager name uniqueness for `FDCxCapSense`.
- Record owner-side hardware validation evidence if available.
