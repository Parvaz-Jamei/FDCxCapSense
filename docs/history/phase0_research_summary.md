# Phase 0 Research Summary

## Goal

Phase 0 established the scientific, legal, and architectural foundation before implementation. It intentionally avoided production driver work at that time. Phase 1, Phase 2, and Phase 3 v6 alignment are now implemented on top of that foundation.

## Fixed management rule

Before writing meaningful driver code:

1. Search current official sources.
2. Study the relevant datasheets and official docs.
3. Register every influential source in `docs/references.md`.
4. Review license and reuse status.
5. Only then implement clean-room code.

## Key decisions

| Area | Decision |
|---|---|
| First chip | FDC1004 / FDC1004-Q1 documentation is the first technical focus. |
| First implementation | Phase 1 implemented Foundation + FDC1004 Core Driver only. |
| CAPDAC ambiguity | Phase 1.1 separated CAPDAC disabled (`CHB=b111`) from CAPDAC enabled (`CHB=b100`). |
| Phase 2 scope | Calibration, diagnostics, baseline, reference compensation, temperature compensation, and profile helpers are allowed as infrastructure only. |
| FDC2x1x | Deferred because it uses a different resonant sensing architecture and should not be mixed into the first driver. |
| License | Project uses MIT. GPL/LGPL and unknown-license source-level material must not enter the main codebase without explicit documented approval. |
| Reuse | Existing libraries are inspection/research-only unless a future documented reuse decision is made. |
| Tests | Fake I2C and AUnit-compatible tests are included. Hardware tests are not claimed. |
| Claims | No ice-certainty, product-grade detection, material classification, liquid classification, or automotive profile claims. |

## Main risks

| Risk | Mitigation |
|---|---|
| Accidentally copying existing library code | Clean-room implementation only; keep `docs/references.md` updated. |
| License contamination from GPL/unknown code | Third-party libraries are marked inspection/research-only; no code import or source-level derivation. |
| Mixing FDC1004 and FDC2x1x too early | FDC2x1x kept to notes only until a later phase. |
| CAPDAC hardware-behavior ambiguity | API now separates CAPDAC disabled and enabled paths. |
| Overclaiming applications | README and docs use alpha/research wording and explicitly reject application certainty claims. |
| Hardware assumptions without validation | No hardware testing is claimed in this package. |

## Phase 0 verification status after Roadmap v6 v0.5.2 alignment

- Project skeleton exists and has been extended through Phase 2.
- `library.properties` includes Arduino metadata and one public root include.
- `src/FDCxCapSense.h` is now the public include for the Phase 2 library.
- Examples are no longer placeholders; they are claim-safe and metadata-aware through Phase 3 v6 alignment where profiles are involved.
- License/reuse policy remains strict.
- Third-party source code was not intentionally copied or reused.

## Not performed

- No hardware test claim.
- No FDC2x1x implementation.
- No CI claim unless separately verified by the owner.
