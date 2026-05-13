# Calibration Infrastructure

This package provides baseline tracking, reference compensation, Auto-CAPDAC, and calibration profile serialization.

## Auto-CAPDAC

Auto-CAPDAC is bounded and reports a reason code. The target is residual pF inside the ADC measurement range, not final capacitance pF. The result includes:

- `status`
- `capdacCode`
- `capacitancePf`
- `residualPf`
- `raw24`
- `reason`
- `saturated`

A saturated reading is not returned as success.

## Baseline tracker

The baseline tracker has warmup, stable-sample, noise, learning-rate, and drift limits. It rejects non-finite inputs and avoids quick overwrite by unstable samples.

## Profile storage

`FDC1004CalibrationProfile` has magic, version, CAPDAC codes, per-channel baselines, temperature coefficients, and CRC. Serialization helpers do not require EEPROM, NVS, Flash, filesystem, or any storage dependency.
