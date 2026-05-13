# Phase 2 Design Notes

Phase 2 provides calibration and compensation infrastructure only.

## Phase 2.1 hotfix updates

- `FDC1004_BaselineCalibration` now initializes `FDCxBaselineConfig` before constructing `FDCxBaselineTracker`.
- Conservative power/idle APIs were added:
  - `setRepeatMode(bool enabled)`
  - `stopRepeatedMeasurements()`
  - `idle()`
- `idle()` clears `REPEAT` and measurement trigger bits in the FDC configuration register.
- No true sleep, deep-sleep, current-consumption, or board-level power claim is made.
- Temperature compensation now includes linear and lookup-table interpolation helpers.
- Auto-CAPDAC now documents `targetCenterPf` as raw residual target and reports `residualPf`.

## Auto-CAPDAC semantics

`targetCenterPf` is the desired residual inside the FDC1004 measurement range after CAPDAC offset has been applied. It is not a final capacitance target. The auto-tune helper scans a bounded CAPDAC range and chooses the code whose residual pF is closest to the requested residual target while avoiding saturated readings.

## CAPDAC mode

- `configureSingleEnded(measurement, channel)` uses CAPDAC disabled mode (`CHB=b111`, CAPDAC code 0).
- `configureSingleEndedWithCapdac(measurement, channel, code)` uses CAPDAC enabled mode (`CHB=b100`, CAPDAC code 0..31).
- The older overload with `capdacCode` remains as a compatibility alias for explicit CAPDAC-enabled behavior.
