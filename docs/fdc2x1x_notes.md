# FDC2x1x Notes

v0.6.11 includes a clean-room, frequency-first FDC2x1x Phase 4 foundation. It remains separate from FDC1004 direct CDC, uses derived capacitance only with explicit LC parameters, and is not hardware validated.

## Separation rules

- FDC1004 is direct capacitance-to-digital with CAPDAC and direct pF conversion.
- FDC2x1x is resonant-LC/frequency-first.
- FDC1004 CAPDAC, direct-pF conversion, and Phase 3 profile logic must not be reused as FDC2x1x logic.
- FDC211x and FDC221x have different output widths; v0.6.11 treats FDC211x as 12-bit raw/foundation-only for frequency conversion.

## Non-claims

This package does not claim product readiness, field validation, automotive-grade behavior, or hardware validation for FDC2x1x.
