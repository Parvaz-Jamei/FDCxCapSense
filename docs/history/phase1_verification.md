# Phase 1 / Phase 1.1 Verification Evidence

## Status

Phase 1 established the FDC1004 core driver. Phase 1.1 fixed CAPDAC ambiguity by separating CAPDAC disabled and CAPDAC enabled paths.

## Environment

- Hardware test: NOT RUN — no physical FDC1004 hardware in this environment.
- Arduino CLI compile: NOT RUN — `arduino-cli` was not available in this environment.
- AUnit execution: NOT RUN — AUnit and Arduino/EpoxyDuino test environment were not installed here.
- Host syntax smoke check: RUN with local temporary Arduino/Wire stubs to catch C++ syntax issues only.

## CAPDAC hotfix tests added

- `configureSingleEnded(measurement, channel)` encodes `CHB=b111`, CAPDAC disabled.
- `configureSingleEndedWithCapdac(measurement, channel, 0)` encodes `CHB=b100`, CAPDAC enabled.
- `configureSingleEndedWithCapdac(measurement, channel, 31)` validates the maximum CAPDAC code.
