# Powder / Granular Proxy Profile

## Purpose

Provides an experimental proxy for fill/coverage-like behavior from direct FDC1004 features.

## Required FDCxFeatureVector fields

`compensatedDeltaPf`, `noisePf`, direct FDC1004 metadata.

Current Phase 3 profiles require `FDC1004_DirectCDC + DirectCapacitance`. FDC2x1x/resonant-LC and derived-capacitance inputs are rejected in v0.6.11 unless a future explicit profile path is implemented and validated.

## Config fields

`emptyPf`, `fullPf`, `maxNoisePf`.

## Valid output states

`Empty`, `Partial`, `Full`, `Unstable`, `Invalid`.

## Confidence meaning

Clamped proxy ratio across empty/full span.

## Baseline/reference/temperature requirements

Requires user calibration for geometry and material behavior.

## Failure modes

Rejects derived/FDC2x1x input, non-Ok sample, invalid span, high noise.

## What it does NOT claim

No density, moisture, industrial accuracy, or product-level claim.
