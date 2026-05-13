# Proximity Experimental Profile

## Purpose

Provides near/far candidate output using hysteresis and debounce on direct FDC1004 features.

## Required FDCxFeatureVector fields

`compensatedDeltaPf`, `noisePf`, direct FDC1004 sample metadata.

Current Phase 3 profiles require `FDC1004_DirectCDC + DirectCapacitance`. FDC2x1x/resonant-LC and derived-capacitance inputs are rejected in v0.6.11 unless a future explicit profile path is implemented and validated.

## Config fields

`nearThresholdPf`, `farThresholdPf`, `debounceSamples`, `minConfidenceDeltaPf`.

## Valid output states

`Near`, `Far`, `Unstable`, `Invalid`.

## Confidence meaning

Based on threshold margin versus noise.

## Baseline/reference/temperature requirements

Baseline should be handled before the feature vector is built.

## Failure modes

Rejects derived/FDC2x1x input, non-Ok sample, invalid thresholds, unstable diagnostics.

## What it does NOT claim

No gesture promise, no presence guarantee, no product-ready detection claim.
