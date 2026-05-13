# Profile Compatibility Matrix

Current experimental application profiles are FDC1004-direct-capacitance profiles. They intentionally do not accept FDC2x1x resonant-frequency or derived-capacitance samples in v0.6.11.

| Measurement source | Current profile support | Reason |
|---|---:|---|
| FDC1004 direct capacitance | Supported | Native direct CDC model used by current FDC1004 driver |
| FDC1004-Q1 direct capacitance | Supported at code level | Same direct-CDC model; no automotive validation claim |
| FDC2x1x resonant frequency | Rejected by current profiles | Phase 4 implements a frequency-first foundation, but these profiles are not frequency-profile implementations |
| FDC2x1x derived capacitance | Rejected by current profiles | Derived capacitance is model-dependent and not equivalent to FDC1004 direct capacitance |
| Unknown family/model | Rejected | Prevents silent misuse of profile logic |
| Non-Ok sample status | Rejected | Prevents stale values from failed measurements |

## Legacy/helper profile APIs

Official examples use `FDCxSample` / `FDCxFeatureVector`. Raw-float helper APIs remain only for compatibility and can be hidden by defining:

```cpp
#define FDCX_DISABLE_LEGACY_PROFILE_HELPERS
```

Do not document raw-float helpers as the official profile API.

Negative/disclaimer text in documentation is allowed. Positive claims such as safety certification, field validation, or automotive-grade behavior are forbidden unless independently validated and approved in a future release.
