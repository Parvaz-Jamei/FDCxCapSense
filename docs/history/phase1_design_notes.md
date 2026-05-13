# Phase 1 Design Notes

## Scope

Phase 1 implemented a basic FDC1004 register-level driver only. It did not include application classifiers, FDC2x1x, advanced calibration, drift/temperature compensation, or hardware validation claims.

## Phase 1.1 CAPDAC hotfix

The original Phase 1 API could configure single-ended measurements with `capdacCode=0` and still encode `CHB=b100` CAPDAC mode. That was ambiguous because CAPDAC enabled and disabled modes can affect shield behavior differently.

Phase 1.1 makes CAPDAC explicit:

| API | Encoding | Meaning |
|---|---|---|
| `configureSingleEnded(measurement, channel)` | `CHB=b111`, `CAPDAC=0` | CAPDAC disabled path |
| `configureSingleEndedWithCapdac(measurement, channel, code)` | `CHB=b100`, `CAPDAC=0..31` | CAPDAC enabled path |
| `configureSingleEnded(measurement, channel, code)` | `CHB=b100`, `CAPDAC=0..31` | Backward-compatible CAPDAC-enabled alias |

Tests now cover:

- Single-ended without CAPDAC => `CHB=b111`
- Single-ended with CAPDAC code `0` => `CHB=b100`
- Single-ended with CAPDAC code `31` => valid maximum encoding

## I2C behavior

The library bounds measurement polling with `waitForMeasurement(..., timeoutMs)`. Low-level `Wire` bus hang behavior may still depend on the Arduino core and target board. Hardware validation must check I2C recovery/timeout behavior on the target board/core.
