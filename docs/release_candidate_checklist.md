# Release Candidate Checklist

| Check | Command / File | Status |
|---|---|---|
| Clean package | `tools/clean_package_check.sh` | Required |
| Claim safety | `tools/claim_safety_scan.sh` | Required |
| Architecture contracts | `scripts/architecture_contract_scan.sh` | Required |
| Host smoke compile | `tools/host_smoke_compile.sh` | Required where C++ toolchain exists |
| Arduino CLI UNO compile | `scripts/compile_examples_arduino_cli.sh arduino:avr:uno` | Required in CI/owner env |
| Arduino CLI SAMD compile | `scripts/compile_examples_arduino_cli.sh arduino:samd:mkrzero` | Required in CI/owner env |
| Arduino CLI ESP32 compile | `scripts/compile_examples_arduino_cli.sh esp32:esp32:esp32` | Required in CI/owner env |
| Metadata | `library.properties` | Review before tag |
| Hardware validation | `docs/hardware_validation_matrix.md` | Owner-side only |
| Library Manager readiness | `docs/library_manager_checklist.md` | Review before submission |

## Release blockers

- Generated artifacts in ZIP.
- Positive safety/product/field claims.
- Missing public repo URL before Library Manager submission.
- Broken examples in CI.
- Unlogged hardware claims.

## v0.6.8 publish-polish gates

- FDC1004 conservative metadata: non-Ok direct samples must not set `hasCapacitance` or `capacitanceIsDirect`.
- FDC1004 identification semantics: wrong manufacturer/device identity must return `InvalidDeviceId`.
- Legacy profile helper APIs are compatibility-only and may be hidden with `FDCX_DISABLE_LEGACY_PROFILE_HELPERS`.
- Library Manager submission remains blocked until real public repository URL, name uniqueness check, tag, CI, and Arduino Lint are complete.


## v0.6.11 industrial hardening gates

- ERROR_CONFIG reserved-bit validation present.
- High-level FDC2x1x configuration setters reject active-mode writes.
- FDC2x1x derived-capacitance sample confidence clamps NaN to zero.
- Simple readFrequencyHz overload is legacy-marked and examples prefer readFrequencyHzFromRegisters().
- FDC211x raw/foundation-only scope is still explicit.
- Owner-side repo URL, Arduino CI and hardware smoke remain release gates.


## v0.6.11 final code-level gates

- `startContinuous()` rejected high-current target-channel requests before state mutation.
- `setClockDividers()` is Sleep-mode guarded; `writeRawClockDividers()` remains explicit raw access.
- Legacy frequency metadata keeps decoded `frefDivider`.
- Automotive timestamp logic is unsigned-delta / wrap-safe.
- `configureSingleChannel()` prevalidates public arguments before entering Sleep Mode.
- Host executable unit tests cover the v0.6.11 edge cases.
