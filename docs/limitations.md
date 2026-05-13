# Limitations

FDCxCapSense v0.6.11 is a public release candidate / owner handoff package. It is not a final stable API release and is not hardware-validated by the Developer.

## FDC1004

- FDC1004 support is direct capacitance-to-digital driver foundation plus calibration and diagnostics helpers.
- Application profiles are experimental and metadata-aware.
- CAPDAC, baseline, reference compensation, and temperature compensation still require owner-side hardware validation.

## FDC2x1x / FDC2214 family

- FDC2x1x/FDC2214 support is implemented as a Phase 4 frequency-first foundation.
- FDC2212/FDC2214 frequency conversion uses the user-supplied effective reference clock.
- FDC2112/FDC2114 raw register support is present, but precise frequency conversion is `NotSupported` in v0.6.x.
- Derived capacitance is model-dependent and requires valid LC parameters.
- `FDC2x1xLCParams.traceReferenceClockHz` is trace metadata only; derived capacitance uses already-converted `frequencyHz`.
- FDC2x1x support is not hardware-validated, not a final field workflow, and not a product interpretation layer.
- Chip-level INTB/DRDY capability is documented separately from driver-level interrupt callbacks; this driver does not implement interrupt callbacks in v0.6.x.

## Profiles and claims

- Profiles provide candidate states, confidence, reason, and raw metrics only.
- No safety, field, automotive, liquid, ice, frost, material, or actuator decision is provided.
- Hardware validation is owner-side and must be documented before any application claim.

## Publish blockers

- The public repository URL in `library.properties` is still a placeholder until the owner creates the repository.
- Arduino CLI compile, Arduino Lint, and hardware validation must be run/recorded owner-side before public submission.
