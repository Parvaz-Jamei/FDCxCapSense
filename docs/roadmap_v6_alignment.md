# Roadmap v6 Alignment — v0.6.11

v0.6.11 is a Strict Sensor Correctness Patch package. It keeps the Roadmap v6 principle: unified brand, separated physics, separated implementation.

## Current state

- Phase 1-3 FDC1004 driver, calibration, diagnostics and experimental profiles remain metadata-aware.
- Phase 3 FDC1004 profiles continue to reject FDC2x1x/resonant-LC and derived-capacitance samples.
- Phase 4 adds a separate FDC2x1x frequency-first foundation.
- The FDC2x1x implementation is not hardware-validated and is not a final public release.

## Transitional source layout

Some v6 folders still provide compatibility forwarding to the earlier `src/fdcx/` implementation for FDC1004 modules. This is documented as transitional. The Phase 4 FDC2x1x foundation lives under `src/devices/fdc2x1x/` and `src/calibration/fdc2x1x/` and must not include or reuse FDC1004 CAPDAC/direct-pF implementation logic.

## Header budget

- `FDCxCapSense_FDC1004.h`: lightweight FDC1004 direct-CDC include.
- `FDCxCapSense_FDC2x1x.h`: lightweight FDC2x1x frequency-first include.
- `FDCxCapSense.h`: full include with optional experimental profiles.
