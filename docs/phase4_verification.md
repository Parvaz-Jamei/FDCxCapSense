# Phase 4 Verification (v0.6.11)

This package bundles reproducible scripts under `tools/` and `scripts/`.

| Check | Status |
|---|---|
| `tools/clean_package_check.sh` | PASS in package build environment |
| `tools/claim_safety_scan.sh` | PASS in package build environment; negative disclaimers are allowed |
| `scripts/architecture_contract_scan.sh` | PASS in package build environment |
| `tools/host_smoke_compile.sh` | PASS in package build environment |
| Arduino CLI example compile | NOT RUN in package build environment if `arduino-cli` is unavailable |
| Hardware validation | NOT PERFORMED / owner-side |

`tools/host_smoke_compile.sh` checks syntax with lightweight Arduino/Wire/AUnit stubs and runs a native smoke executable for key architecture contracts. It does not replace Arduino CLI or hardware validation.

`scripts/compile_examples_arduino_cli.sh <fqbn>` performs real Arduino CLI example compilation when `arduino-cli` is available. If unavailable, it reports `NOT RUN` honestly rather than pretending success.
