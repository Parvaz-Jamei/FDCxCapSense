# Liquid Level Experimental Profile

## Purpose

Estimates a calibrated fill percentage from direct FDC1004 capacitance features.

## Required FDCxFeatureVector fields

`sample`, `baselinePf`, `deltaPf`, `compensatedDeltaPf`, optional `referenceDeltaPf`, `baselineLearned`.

Current Phase 3 profiles require `FDC1004_DirectCDC + DirectCapacitance`. FDC2x1x/resonant-LC and derived-capacitance inputs are rejected in v0.6.11 unless a future explicit profile path is implemented and validated.

## Config fields

`emptyPf`, `fullPf`, `useReferenceChannel`, `referenceScale`, `minValidSpanPf`, `referenceBaselinePf`, `referenceBaselineValid`.

## Valid output states

`Empty`, `Partial`, `Full`, `Unknown`, `Invalid`.

## Confidence meaning

Higher when calibrated span is large relative to noise.

## Baseline/reference/temperature requirements

Requires learned baseline. If reference compensation is enabled, reference baseline/reference delta must be valid.

## Failure modes

Rejects derived/FDC2x1x input, non-Ok sample, invalid span, missing baseline, missing reference baseline.

## What it does NOT claim

No millimeter accuracy, no product-level liquid measurement, no field validation claim.


## v0.6.11 legacy temperature helper note

The legacy raw-float `update(levelPf, referencePf, temperatureC, baseline, flags)` helper stores `temperatureC` as metadata only. It does not perform liquid-level temperature compensation. For official flows, apply compensation before creating `FDCxFeatureVector` and use `update(FDCxFeatureVector)`.
