# Release Process

This project should be released by the owner.

## Current package

- Package / release target: `FDCxCapSense v0.6.13`
- Status: Arduino Library Manager submission candidate
- Submission status: in progress
- Hardware validation: owner-side only; not claimed unless evidence is logged

## Required checks before tag

Run through GitHub Actions or locally:

```bash
bash ./tools/clean_package_check.sh
bash ./tools/claim_safety_scan.sh
bash ./scripts/architecture_contract_scan.sh
bash ./tools/host_smoke_compile.sh
bash ./tools/host_unit_tests.sh
```

If Arduino CLI is available:

```bash
bash ./scripts/compile_examples_arduino_cli.sh arduino:avr:uno
bash ./scripts/compile_examples_arduino_cli.sh arduino:samd:mkrzero
bash ./scripts/compile_examples_arduino_cli.sh esp32:esp32:esp32
```

## Versioning

- Use `0.6.13` for the corrected Arduino Library Manager submission candidate.
- Do not reuse an old tag after fixing library metadata.
- If a submission needs another correction, bump the version and create a new tag.
- Do not tag `1.0.0` until API stability, documentation maturity, and owner-side hardware validation justify it.

## Tagging and Library Manager

Arduino Library Manager depends on a public repository, valid metadata, and SemVer-compatible tags/releases.

If a correction is needed after a tag:

1. Fix the library repository.
2. Increment `version` in `library.properties`.
3. Create a new tag or release.
4. Re-run GitHub Actions.
5. Mention `@ArduinoBot` in the Library Registry PR if a re-check is needed.

## Owner-side release steps

1. Confirm `library.properties` is valid.
2. Confirm repository URL is correct.
3. Run CI and local checks.
4. Run Arduino Lint.
5. Review claim wording.
6. Create release notes.
7. Create tag/release `v0.6.13`.
8. Submit to Library Manager only after checks pass.

## Maintainer / project links

- Website: https://proio.ir
- LinkedIn: https://www.linkedin.com/in/parvaz-jamei
