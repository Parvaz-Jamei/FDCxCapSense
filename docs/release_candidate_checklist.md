# Release Candidate Checklist - v0.6.13

| Check | Command / File | Status |
|---|---|---|
| Clean package | `tools/clean_package_check.sh` | Required |
| Claim safety | `tools/claim_safety_scan.sh` | Required |
| Architecture contracts | `scripts/architecture_contract_scan.sh` | Required |
| Host smoke compile | `tools/host_smoke_compile.sh` | Required |
| Host unit tests | `tools/host_unit_tests.sh` | Required |
| Arduino CLI UNO compile | `scripts/compile_examples_arduino_cli.sh arduino:avr:uno` | Required in CI/owner env |
| Arduino CLI SAMD compile | `scripts/compile_examples_arduino_cli.sh arduino:samd:mkrzero` | Required in CI/owner env |
| Arduino CLI ESP32 compile | `scripts/compile_examples_arduino_cli.sh esp32:esp32:esp32` | Required in CI/owner env |
| Arduino Lint strict | `.github/workflows/arduino-lint.yml` | Required |
| Metadata | `library.properties` | Must be valid multi-line key/value metadata |
| Hardware validation | `docs/hardware_validation_matrix.md` | Owner-side only |
| Library Manager readiness | `docs/library_manager_checklist.md` | Review before submission |

## Release blockers

- Malformed `library.properties`.
- Generated artifacts in ZIP/repo.
- Positive safety/product/field claims without evidence.
- Broken examples in CI.
- Failed Arduino Lint strict.
- Failed compile matrix.
- Unlogged hardware claims.

## v0.6.13 final submission gates

- `library.properties` version is `0.6.13`.
- Repository URL is `https://github.com/Parvaz-Jamei/FDCxCapSense`.
- GitHub Actions pass:
  - Unit and static checks
  - Arduino Lint
  - Compile examples
- FDC211x remains raw/foundation-only in v0.6.x.
- Hardware validation is not claimed unless owner evidence exists.
- Tag/release `v0.6.13` is created after the metadata fix.
