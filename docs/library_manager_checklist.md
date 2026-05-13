# Arduino Library Manager Checklist - v0.6.13

This checklist prepares FDCxCapSense for Arduino Library Manager submission.

| Gate | Status | Notes |
|---|---|---|
| `library.properties` exists at root | PASS | Version is `0.6.13`. |
| Library name | OWNER VERIFY | `FDCxCapSense`; owner must verify name uniqueness before submission. |
| Public repository URL | PASS | `https://github.com/Parvaz-Jamei/FDCxCapSense` |
| Repository root is library root | PASS / OWNER VERIFY | `library.properties`, `src/`, `examples/`, and `LICENSE` must be at repository root. |
| Arduino Lint strict | OWNER RUN REQUIRED | Run GitHub Actions before submission. |
| Arduino CLI compile matrix | OWNER RUN REQUIRED | UNO, SAMD, and ESP32 matrix must pass. |
| Host checks | PASS / OWNER VERIFY | Unit/static, host smoke compile, and host unit tests should pass in CI. |
| Hardware validation | OWNER-SIDE ONLY | Do not claim hardware validation until evidence is logged. |
| License and clean-room policy | PASS | No third-party code reuse without documented approval. |

## Owner release sequence

1. Confirm `library.properties` is valid multi-line key/value metadata.
2. Confirm the public repository exists at `https://github.com/Parvaz-Jamei/FDCxCapSense`.
3. Confirm the repository root is the Arduino library root, not a subfolder.
4. Run GitHub Actions:
   - Unit and static checks
   - Arduino Lint
   - Compile examples
5. Check Arduino Library Manager name uniqueness for `FDCxCapSense`.
6. Create tag/release `v0.6.13`.
7. Submit to Arduino Library Manager registry only after CI and lint pass.
8. If any correction is needed after a tag, bump version and create a new tag.

## GitHub About section

Suggested repository description:

```text
Arduino library for TI FDC1004, FDC1004-Q1 and FDC2x1x/FDC2214 capacitive sensing with calibration, CAPDAC, diagnostics, liquid level and proximity profiles.
```

Suggested topics:

```text
arduino
arduino-library
capacitive-sensing
fdc1004
fdc2214
fdc2x1x
i2c
esp32
liquid-level-sensing
proximity-sensing
fdc1004-q1
fdc2212
fdc2114
fdc2112
sensor-calibration
industrial-iot
embedded
```

## Submission blockers

- `library.properties` is malformed.
- Repository URL in `library.properties` does not match the public repository.
- GitHub Actions compile matrix has not passed.
- Arduino Lint has not passed.
- Release tag `v0.6.13` is not created.
- Library name collision has not been checked.
- Hardware validation is claimed without evidence.
