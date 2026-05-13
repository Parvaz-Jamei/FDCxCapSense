# FDC2x1x Limitations (v0.6.11 Real-Hardware Startup Patch)

- FDC211x precise frequency conversion is not implemented in v0.6.11; only 12-bit raw/foundation behavior is provided.
- FDC221x frequency conversion is supported through `readFrequencyHzFromRegisters()` and the explicit compatibility overloads.
- FDC2x1x capacitance is derived from LC assumptions; it is not equivalent to FDC1004 direct capacitance.
- Public examples now start conversion with `startContinuous()`, but values such as RCOUNT, SETTLECOUNT, clock dividers, drive-current code and deglitch bandwidth are still example values only.
- Hardware validation is owner-side. The Developer did not run real board testing in this package.
- Low-level raw register writes remain available for advanced users, but public examples use safe helpers for CONFIG/MUX_CONFIG-sensitive flow.


## v0.6.11 hardening note

FDC2112/FDC2114 remain raw/foundation-only in v0.6.x. Precise frequency conversion is intentionally not implemented for FDC211x in this release. High-level FDC2x1x configuration setters require Sleep Mode; use the documented sleep/configure/start flow.
