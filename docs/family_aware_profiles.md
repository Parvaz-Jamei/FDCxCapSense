# Family-Aware Profiles

All official profile paths use `FDCxFeatureVector` or `FDCxSample`, not raw `float pF`. Legacy raw-float helpers may remain for old sketches, but official examples and architecture tests use metadata-aware input.

## Required validation sequence

Each current profile must reject inputs that fail any of these checks:

1. Family/model metadata present.
2. Direct FDC1004 capacitance input.
3. Sample status is `Ok`.
4. Required baseline/reference/temperature fields are available.
5. Diagnostics do not indicate an unstable or unavailable measurement.

## Current support matrix

| Profile | Current supported input | Derived/FDC2x1x input | Notes |
|---|---|---|---|
| Liquid level experimental | FDC1004 direct capacitance feature vector | Rejected | Requires baseline; reference optional but must be valid when enabled |
| Proximity experimental | FDC1004 direct capacitance feature vector | Rejected | Hysteresis/debounce only |
| Surface state experimental | FDC1004 direct capacitance feature vector | Rejected | Candidate-only output; temperature-gated |
| Material feature helper | FDC1004 direct capacitance feature vector | Rejected | Feature extraction only |
| Powder/granular proxy | FDC1004 direct capacitance feature vector | Rejected | Proxy/stability helper only |
| Automotive proximity demo | FDC1004 direct capacitance feature vector | Rejected | Demo/candidate only, no actuator or safety output |
