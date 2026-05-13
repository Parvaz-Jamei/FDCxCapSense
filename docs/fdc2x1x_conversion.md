# FDC2x1x Conversion Notes

FDC2x1x primary output is resonant frequency/raw code, not direct capacitance. The library keeps this separate from FDC1004 direct capacitance measurements.

## FDC2212 / FDC2214 path

The v0.6.11 driver implements the FDC221x 28-bit path:

1. Read `DATA_CHx` MSB.
2. Read `DATA_LSB_CHx`.
3. Combine into a 28-bit raw code.
4. Convert to frequency from the user-supplied effective reference clock.

This produces a `FDC2x1xFrequency` sample with:

- `family = FDC2x1x_ResonantLC`
- `model = ResonantFrequency`
- `hasFrequency = true` only when status is `Ok`
- `hasCapacitance = false`

## FDC2112 / FDC2114 path

The v0.6.11 driver implements FDC211x raw register access as a 12-bit foundation path. Precise `readFrequencyHz()` conversion for FDC211x returns `NotSupported` because variant-specific gain/offset behavior must be modeled explicitly before public use.

Current behavior:

- `readRaw()` returns the 12-bit data from `DATA_CHx`.
- `readFrequencyHz()` returns `NotSupported` for FDC211x.
- `DATA_LSB_CHx` is not read for FDC211x.

## Derived capacitance

Derived capacitance uses the converted `frequency.frequencyHz` and user-supplied LC parameters:

- `inductanceUH`
- `parasiticCapacitancePf`

`traceReferenceClockHz` in `FDC2x1xLCParams` is optional trace metadata for examples/logging; the derive helper does not use it directly because the input frequency is already converted.

Derived samples are marked with:

- `model = DerivedCapacitance`
- `capacitanceIsDirect = false`
- `capacitanceIsDerived = true` only when status is `Ok`

FDC1004 Phase 3 profiles reject FDC2x1x derived capacitance samples unless a future explicit compatible profile path is added.


## v0.6.11 clock-divider and reference-clock path

For FDC2212/FDC2214, `readFrequencyHzFromRegisters(channel, fclkHz, output)` reads `CLOCK_DIVIDERS_CHx`, decodes `CHx_FIN_SEL` from bits `[13:12]`, decodes `CHx_FREF_DIVIDER` from bits `[9:0]`, computes `fREFx = fCLK / CHx_FREF_DIVIDER`, and then computes:

```text
fSENSORx = finSelMultiplier * fREFx * DATAx / 2^28
```

`CHx_FREF_DIVIDER == 0` and invalid `CHx_FIN_SEL` encodings are rejected. FDC2112/FDC2114 remain raw/foundation-only for precise frequency conversion in v0.6.x and return `NotSupported` for the precise conversion path.

DATA MSB error flags are checked before masking raw bits. `CHx_ERR_WD` or `CHx_ERR_AW` makes `readRaw()` return `MeasurementNotReady` instead of treating the masked DATA field as valid.


## Startup flow

FDC2x1x devices power up in Sleep Mode. Configure channel timing/current/deglitch while asleep, then call `startContinuous(channel)` to select `ACTIVE_CHAN` and clear `CONFIG.SLEEP_MODE_EN`. `begin()` performs identity checks only and does not claim conversion has started.

For real hardware, prefer `readFrequencyHzFromRegisters()`, because it decodes `CHx_FIN_SEL` and `CHx_FREF_DIVIDER` from the device registers before computing frequency. The older `readFrequencyHz(channel, frefHz, ...)` overloads are compatibility helpers for callers that already know the effective reference-clock path.


## Reference clock source

When an example or sketch uses an external clock such as `40000000.0f`, it must set `CONFIG.REF_CLK_SRC` explicitly with `setReferenceClockSource(FDC2x1xReferenceClockSource::ExternalClockInput)` before starting conversion. Internal-oscillator use should either keep `REF_CLK_SRC = 0` or document the effective clock value used for frequency calculations.

## Sensor topology and FIN_SEL

Single-ended examples should use `configureClockDividersForSingleEnded()` or the topology-aware `configureClockDividers(channel, FDC2x1xSensorTopology::SingleEnded, frefDivider)` helper. These helpers force the single-ended `FIN_SEL` multiplier used by the library. The older raw multiplier overload remains available for advanced users who have read the datasheet.


## v0.6.11 release edge-case hardening

- The simple `readFrequencyHz(channel, frefHz, output)` overload is retained only as a legacy compatibility helper for callers that already know the effective reference frequency and clock-divider path. Official real-hardware examples use `readFrequencyHzFromRegisters()` so `CHx_FIN_SEL` and `CHx_FREF_DIVIDER` are decoded from the device.
- Differential topology requires an explicit `FIN_SEL` choice through `configureClockDividersForDifferential(channel, finSelMultiplier, frefDivider)`. Single-ended helpers continue to force `FIN_SEL=2`.
- FDC211x stays raw/foundation-only in v0.6.x; no precise FDC211x frequency-conversion claim is added.


## v0.6.11 industrial release hardening

- `ERROR_CONFIG` accepts only documented alert/interrupt routing bits and rejects reserved-bit writes.
- Safe high-level configuration setters require Sleep Mode. Use `stopConversions()` or `configureSingleChannel()` before changing timing, deglitch, drive-current or reference-clock settings.
- The legacy `readFrequencyHz(channel, frefHz, output)` compatibility overload now reads `CHx_FIN_SEL` before calculating; `readFrequencyHzFromRegisters()` remains the recommended real-hardware path because it decodes both `CHx_FIN_SEL` and `CHx_FREF_DIVIDER`.
- FDC2112/FDC2114 remain raw/foundation-only in v0.6.x; FDC2212/FDC2214 are the supported precise frequency-conversion path.


## v0.6.11 final industrial quality polish

`startContinuous()` now preflights `HIGH_CURRENT_DRV` channel compatibility before entering Sleep Mode, so rejected target-channel requests do not mutate the device state. `setClockDividers()` is now a Sleep-mode-guarded compatibility setter; `writeRawClockDividers()` remains the explicit low-level raw path. The legacy `readFrequencyHz(channel, frefHz, output)` helper preserves decoded `frefDivider` metadata, but real hardware examples continue to prefer `readFrequencyHzFromRegisters(channel, fclkHz, output)`.
