# Maintenance Plan

## Support policy

- Treat `v0.x` releases as owner release handoffs / pre-stable packages.
- Preserve the Roadmap v6 principle: unified brand, separated physics, separated implementation.
- Keep FDC1004 direct CDC logic separate from FDC2x1x resonant LC logic.

## Issue triage

Use labels:

- `bug`
- `docs`
- `example`
- `hardware-validation`
- `fdc1004`
- `fdc2x1x`
- `profile`
- `claim-safety`
- `license-review`

## Required evidence for hardware issues

Ask for board, Arduino core version, chip, wiring, example, serial output, environment, and date.

## Future work candidates

- FDC211x precise frequency conversion with official variant-specific parameters.
- Driver-level INTB/DRDY callback API for FDC2x1x.
- Hardware validation matrix entries from owner-side tests.
- More examples only after existing examples compile in CI.

## Non-goals

- No safety system claims.
- No actuator-control decisions.
- No direct reuse of third-party implementation code.
