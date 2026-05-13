# Arduino Library Manager Checklist - v0.6.11 Strict Sensor Correctness Patch

This checklist prepares the library for owner-side public repository setup. It is not a submission record.

| Gate | Status | Notes |
|---|---|---|
| `library.properties` exists at root | PASS | Version is SemVer-compatible: 0.6.11. |
| Library name | PASS / OWNER VERIFY | `FDCxCapSense`; owner must still check Library Manager name uniqueness before submission. |
| Public repository URL | PASS / OWNER VERIFY | `https://github.com/Parvaz-Jamei/FDCxCapSense`; owner must confirm the public repository exists before tag/submission. |
| Repository root is library root | OWNER ACTION | `library.properties`, `src/`, `examples/`, and `LICENSE` must be at repository root. |
| Release tag planned | TODO OWNER | Use a SemVer-compatible tag only after CI, lint, metadata, and docs pass. |
| SEO metadata | PASS | `sentence` and `paragraph` include TI FDC1004, FDC1004-Q1, FDC2x1x, FDC2112, FDC2114, FDC2212, FDC2214, CAPDAC, diagnostics, calibration, and profile terms. |
| Public includes intentional | PASS | `includes=FDCxCapSense.h,FDCxCapSense_FDC1004.h,FDCxCapSense_FDC2x1x.h`. |
| Generated artifacts absent | PASS | Run `tools/clean_package_check.sh`. |
| Arduino Lint workflow | INCLUDED / OWNER RUN REQUIRED | `.github/workflows/arduino-lint.yml` uses the official Arduino lint action pattern; run it on the public repository. |
| Arduino CLI compile matrix | WORKFLOW INCLUDED / OWNER RUN REQUIRED | Run UNO, SAMD, and ESP32 matrix before tag. |
| Hardware validation | OWNER-SIDE ONLY | Do not claim hardware validation until evidence is logged. |
| License and clean-room policy | PASS | Third-party code reuse remains forbidden without documented approval. |

## Owner release sequence

1. Confirm the public repository exists at `https://github.com/Parvaz-Jamei/FDCxCapSense`.
2. Confirm `library.properties` keeps the repository URL set to the public repository.
3. Confirm the repository root is the Arduino library root, not a subfolder.
4. Add the GitHub repository description:

```text
Arduino library for TI FDC1004, FDC1004-Q1 and FDC2x1x/FDC2214 capacitive sensing with calibration, CAPDAC, diagnostics, liquid level, proximity and surface-state profiles.
```

5. Add GitHub topics:

```text
arduino arduino-library capacitive-sensing fdc1004 fdc2214 fdc2x1x i2c esp32 liquid-level-sensing proximity-sensing fdc1004-q1 fdc2212 fdc2114 fdc2112 sensor-calibration industrial-iot embedded
```

6. Run GitHub Actions compile matrix, unit-test smoke, and Arduino Lint.
7. Record owner-side hardware validation if available; otherwise keep `NOT PERFORMED`.
8. Check Arduino Library Manager name uniqueness for `FDCxCapSense`.
9. Create a SemVer tag only after checks pass.
10. Submit to Arduino Library Manager registry only from the public repository.
11. If any correction is needed after a tag, bump version and create a new tag.

## Submission blockers

- Public repository does not exist at the configured URL.
- Repository URL in `library.properties` does not match the public repository.
- GitHub Actions compile matrix has not run.
- Arduino Lint has not run.
- Release tag is not created.
- Library name collision has not been checked.
- Hardware validation is claimed without evidence.


## v0.6.11 owner-side blockers

- Owner must confirm the public repository exists at `https://github.com/Parvaz-Jamei/FDCxCapSense` before tag/submission.
- Arduino CLI / CI and hardware smoke validation remain owner-side gates before Library Manager submission.
