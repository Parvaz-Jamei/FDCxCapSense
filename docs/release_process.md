# Release Process

This project should be released by the owner, not by the Developer.

## Current package

- Package: `FDCxCapSense_v0.6.11_final_industrial_quality_polish.zip`
- Status: Public release candidate / owner handoff
- Submission status: not submitted to Arduino Library Manager
- Hardware validation: owner-side / not performed by the Developer

## GitHub About section

Suggested repository title:

```text
FDCxCapSense — Arduino Library for TI FDC1004 and FDC2x1x Capacitive Sensing
```

Suggested repository description:

```text
Arduino library for TI FDC1004, FDC1004-Q1 and FDC2x1x/FDC2214 capacitive sensing with calibration, CAPDAC, diagnostics, liquid level, proximity and surface-state profiles.
```

Suggested high-priority topics:

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
```

Additional useful topics:

```text
fdc1004-q1
fdc2212
fdc2114
fdc2112
capacitance-sensor
capacitance-to-digital
sensor-calibration
industrial-iot
embedded
touch-sensing
surface-sensing
```

## Pre-release checks

Run:

```bash
tools/clean_package_check.sh
tools/claim_safety_scan.sh
scripts/architecture_contract_scan.sh
tools/host_smoke_compile.sh
scripts/compile_examples_arduino_cli.sh arduino:avr:uno
scripts/compile_examples_arduino_cli.sh arduino:samd:mkrzero
scripts/compile_examples_arduino_cli.sh esp32:esp32:esp32
```

If Arduino CLI is unavailable, record:

```text
Arduino CLI compile: NOT RUN - arduino-cli unavailable in Developer environment. Workflow is provided for CI/owner-side execution.
```

## Versioning

- Use `0.6.11` for this SEO/public-presentation polish package.
- Do not tag `1.0.0` until API stability, documentation maturity, and owner-side hardware validation justify it.

## Tagging and Library Manager

Arduino Library Manager depends on a public repository, valid metadata, and SemVer-compatible tags/releases. If a submission needs correction, bump the version and create a new tag instead of replacing an old tag.

## Owner-side release steps

1. Confirm metadata and repository URL.
2. Check Arduino Library Manager name uniqueness for `FDCxCapSense`.
3. Run CI and local checks.
4. Run Arduino Lint.
5. Review claim wording.
6. Create release notes.
7. Create a Git tag/release.
8. Submit to Library Manager only after the above steps pass.

## Maintainer / project links

- Website: https://proio.ir
- LinkedIn: https://www.linkedin.com/in/parvaz-jamei
