# FDC1004 vs FDC2x1x Architecture

FDC1004 and FDC2x1x are not interchangeable measurement models.

- FDC1004: direct capacitance-to-digital converter. This library implements FDC1004 as a direct CDC driver.
- FDC2x1x/FDC2214: resonant/narrow-band LC sensing architecture. Future support must be frequency-first and must not reuse FDC1004 CAPDAC/direct-pF assumptions.

v0.6.11 Strict Sensor Correctness Patch adds a separate frequency-first FDC2x1x foundation while preserving the architectural separation and family/model metadata contract.
