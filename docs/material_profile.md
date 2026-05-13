# Material Feature Helper

## Purpose

Computes relative features from direct FDC1004 capacitance and optional reference delta.

## Required FDCxFeatureVector fields

`sample.capacitancePf`, `baselinePf`, `deltaPf`, `referenceDeltaPf`, direct FDC1004 metadata.

Current Phase 3 profiles require `FDC1004_DirectCDC + DirectCapacitance`. FDC2x1x/resonant-LC and derived-capacitance inputs are rejected in v0.6.11 unless a future explicit profile path is implemented and validated.

## Config fields

No material classifier config is provided in v0.6.11; this remains feature extraction only.

## Valid output states

Returns `FDCxMaterialSignature` only.

## Confidence meaning

`stabilityScore` is a bounded helper, not a classification confidence.

## Baseline/reference/temperature requirements

Requires a valid baseline/reference workflow chosen by the user.

## Failure modes

Rejects derived/FDC2x1x input, non-Ok sample, non-finite values.

## What it does NOT claim

No material identification or classification claim.
