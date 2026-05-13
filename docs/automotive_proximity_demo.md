# Automotive Proximity Demo

## Purpose

Demonstrates proximity candidate logic with debounce and timestamp gating.

## Required FDCxFeatureVector fields

`compensatedDeltaPf`, `noisePf`, direct FDC1004 metadata, `timestampMs`.

Current Phase 3 profiles require `FDC1004_DirectCDC + DirectCapacitance`. FDC2x1x/resonant-LC and derived-capacitance inputs are rejected in v0.6.11 unless a future explicit profile path is implemented and validated.

## Config fields

`handPresenceThresholdPf`, `touchlessCandidateThresholdPf`, `debounceSamples`, `maxNoisePf`, `minCandidateIntervalMs`.

## Valid output states

`Far`, `Near`, `Candidate`, `Unstable`, `Invalid`.

## Confidence meaning

Based on margin versus threshold and noise.

## Baseline/reference/temperature requirements

Requires monotonic timestamps for demo event gating.

## Failure modes

Rejects derived/FDC2x1x input, non-Ok sample, invalid thresholds, high noise, non-monotonic timestamp.

## What it does NOT claim

No automotive-grade claim, no actuator output, no safety trigger, no ASIL/production claim.
