# Contributing to FDCxCapSense

Thank you for considering a contribution.

## Development rules

- Research first: use official TI and Arduino documentation before implementation.
- Keep FDC1004 direct-CDC logic separate from FDC2x1x resonant-LC logic.
- Keep all application profiles metadata-aware.
- Do not add safety, field, or product claims.
- Do not copy, adapt, port, translate, paraphrase, closely rewrite, or derive from third-party code, examples, tests, documentation text, API shape, file layout, naming pattern, or recognizable implementation logic unless license review, approval, compatibility check, and attribution are documented first.

## Required checks before a pull request

```bash
tools/clean_package_check.sh
tools/claim_safety_scan.sh
scripts/architecture_contract_scan.sh
tools/host_smoke_compile.sh
```

When available, also run Arduino CLI example compile:

```bash
scripts/compile_examples_arduino_cli.sh arduino:avr:uno
scripts/compile_examples_arduino_cli.sh arduino:samd:mkrzero
scripts/compile_examples_arduino_cli.sh esp32:esp32:esp32
```

## Hardware reports

Use the hardware validation issue template and include board, core version, chip, wiring, example, serial output, and date.
