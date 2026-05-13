# Surface State Experimental Profile

## Purpose

Outputs dry/wet/frost/ice candidate states from direct FDC1004 features.

## Required FDCxFeatureVector fields

`compensatedDeltaPf`, `temperatureC`, `hasTemperature`, `noisePf`, `driftPf`, `baselineLearned`, diagnostics.

Current Phase 3 profiles require `FDC1004_DirectCDC + DirectCapacitance`. FDC2x1x/resonant-LC and derived-capacitance inputs are rejected in v0.6.11 unless a future explicit profile path is implemented and validated.

## Config fields

`wetDeltaThresholdPf`, `frostDeltaThresholdPf`, `iceDeltaThresholdPf`, `maxNoisePf`, `minConfidence`, `requireTemperatureInput`, `frostMaxTemperatureC`, `iceMaxTemperatureC`.

## Valid output states

`Dry`, `WetCandidate`, `FrostCandidate`, `IceCandidate`, `Unknown`, `Unstable`, `Invalid`.

## Confidence meaning

Based on threshold margin and noise, clamped to 0..1.

## Baseline/reference/temperature requirements

Requires learned baseline. Requires valid temperature when `requireTemperatureInput=true`. Warm temperature must not produce frost/ice candidates.

## Failure modes

Rejects derived/FDC2x1x input, non-Ok sample, missing temperature, missing baseline, high noise, invalid threshold order.

## What it does NOT claim

No black-ice certainty, no safe-road/unsafe-road decision, no safety system.
