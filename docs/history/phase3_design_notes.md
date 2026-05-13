# Phase 3 Design Notes

This Phase 3 component is experimental and calibration-aware. It returns status, state, confidence, reason, and raw metrics. It does not provide final product or safety decisions.

Inputs must be finite and validated by the caller. Synthetic tests cover thresholds, invalid inputs, confidence bounds, and claim-safe behavior where applicable. Hardware validation is outside this package evidence.
